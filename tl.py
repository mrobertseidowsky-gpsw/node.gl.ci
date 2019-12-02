#!/usr/bin/env python
# -*- coding: utf-8 -*-

import Tkinter as tk
import ttk

import pynodegl as ngl
from pynodegl_utils.misc import Media


_UNIT_BASE = 100


class _NGLWidget(tk.Canvas):

    def __init__(self, master, media):
        tk.Canvas.__init__(self, master)
        self._viewer = ngl.Viewer()
        self._time = 0
        self._media = media

        # Fake highlights
        import random
        random.seed(0)
        self._highlights = [random.uniform(0, self._media.duration) for i in range(5)]

        self.bind("<Configure>", self._configure)
        self.bind("<Expose>", self._draw)
        #self.bind("<Activate>", self._draw)
        #self.bind("<Deactivate>", self._draw)
        #self.bind("<Enter>", self._draw)
        #self.bind("<Leave>", self._draw)
        #self.bind("<Visibility>", self._draw)
        #self.bind("<FocusIn>", self._draw)
        #self.bind("<FocusOut>", self._draw)
        #self.bind("<ResizeRequest>", self._draw)

    def seek(self, value):
        self._time = self._media.duration * float(value) / _UNIT_BASE
        self._refresh()

    @property
    def highlights(self):
        return self._highlights

    @staticmethod
    def _get_viewport(width, height, aspect_ratio):
        view_width = width
        view_height = width * aspect_ratio[1] / aspect_ratio[0]
        if view_height > height:
            view_height = height
            view_width = height * aspect_ratio[0] / aspect_ratio[1]
        view_x = (width - view_width) // 2
        view_y = (height - view_height) // 2
        return (view_x, view_y, view_width, view_height)

    @staticmethod
    def _get_scene(media):
        q = ngl.Quad((-1, -1, 0), (2, 0, 0), (0, 2, 0))
        m = ngl.Media(media.filename)
        t = ngl.Texture2D(data_src=m)
        render = ngl.Render(q)
        render.update_textures(tex0=t)
        return render

    def _configure(self, event):
        self._wid = self.winfo_id()
        self._width = self.winfo_width()
        self._height = self.winfo_height()
        self._aspect_ratio = self._media.dimensions

        self._viewer.configure(window=self._wid,
                         width=self._width,
                         height=self._height,
                         viewport=self._get_viewport(self._width, self._height, self._aspect_ratio),
                         swap_interval=1)
        scene = self._get_scene(self._media)
        self._viewer.set_scene(scene)
        self._refresh()

    def _refresh(self):
        #print 'draw at time', self._time
        self._viewer.draw(self._time)

    def _draw(self, event):
        self._refresh()


class _Segment:

    _EDGE_W = 10
    _CENTER_W = 20

    W = _CENTER_W + _EDGE_W * 2

    def __init__(self, canvas, x, color):
        self._canvas = canvas

        y1 = canvas.y1
        y2 = canvas.y2

        edge_l_x1 = x
        edge_l_x2 = x + self._EDGE_W

        center_x1 = edge_l_x2
        center_x2 = edge_l_x2 + self._CENTER_W

        edge_r_x1 = center_x2
        edge_r_x2 = center_x2 + self._EDGE_W

        self._center = canvas.create_rectangle(center_x1, y1, center_x2, y2, fill=color)
        self._edge_l = canvas.create_rectangle(edge_l_x1, y1, edge_l_x2, y2, fill=color, stipple='gray25')
        self._edge_r = canvas.create_rectangle(edge_r_x1, y1, edge_r_x2, y2, fill=color, stipple='gray25')

        self.x1 = edge_l_x1
        self.x2 = edge_r_x2

        canvas.tag_bind(self._edge_l, '<B1-Motion>', self._resize_left)
        canvas.tag_bind(self._edge_r, '<B1-Motion>', self._resize_right)
        canvas.tag_bind(self._center, '<B1-Motion>', self._move)

        #print(f'Created segment ({self.x1}->{self.x2})')

    def remove(self):
        self._canvas.delete(self._center)
        self._canvas.delete(self._edge_l)
        self._canvas.delete(self._edge_r)

    def update_y(self, y1, y2):
        for shape in {self._edge_l, self._edge_r, self._center}:
            x1, _, x2, _ = self._canvas.coords(shape)
            self._canvas.coords(shape, x1, y1, x2, y2)

    def _move(self, event):
        half_w = (self.x2 - self.x1) / 2

        min_x = self._canvas.get_min_x(self.x1, segment_exclude=self) + half_w
        max_x = self._canvas.get_max_x(self.x2, segment_exclude=self) - half_w

        #print(f'x:{event.x} min_x:{min_x} max_x:{max_x}')
        x = min(max(event.x, min_x), max_x)

        center_x1 = x - half_w + self._EDGE_W
        center_x2 = x + half_w - self._EDGE_W

        edge_l_x2 = center_x1
        edge_l_x1 = center_x1 - self._EDGE_W

        edge_r_x1 = center_x2
        edge_r_x2 = center_x2 + self._EDGE_W

        y1, y2 = self._canvas.y1, self._canvas.y2
        self._canvas.coords(self._edge_l, edge_l_x1, y1, edge_l_x2, y2)
        self._canvas.coords(self._edge_r, edge_r_x1, y1, edge_r_x2, y2)
        self._canvas.coords(self._center, center_x1, y1, center_x2, y2)

        self.x1 = edge_l_x1
        self.x2 = edge_r_x2

    def _resize_right(self, event):
        min_x = self.x1 + self.W
        max_x = self._canvas.get_max_x(self.x2, segment_exclude=self)

        #print(f'x:{event.x} min_x:{min_x} max_x:{max_x}')
        x = min(max(event.x, min_x), max_x)

        center_x1, y1, _, y2 = self._canvas.coords(self._center)
        edge_r_x2 = x
        edge_r_x1 = edge_r_x2 - self._EDGE_W
        center_x2 = edge_r_x1
        self._canvas.coords(self._center, center_x1, y1, center_x2, y2)
        self._canvas.coords(self._edge_r, edge_r_x1, y1, edge_r_x2, y2)
        self.x2 = x

    def _resize_left(self, event):
        min_x = self._canvas.get_min_x(self.x1, segment_exclude=self)
        max_x = self.x2 - self.W

        #print(f'x:{event.x} min_x:{min_x} max_x:{max_x}')
        x = min(max(event.x, min_x), max_x)

        _, y1, center_x2, y2 = self._canvas.coords(self._center)
        edge_l_x1 = x
        edge_l_x2 = edge_l_x1 + self._EDGE_W
        center_x1 = edge_l_x2
        self._canvas.coords(self._edge_l, edge_l_x1, y1, edge_l_x2, y2)
        self._canvas.coords(self._center, center_x1, y1, center_x2, y2)
        self.x1 = x


class _Highlight:

    _W = 10
    _H = 10
    _PAD = 5

    def __init__(self, canvas, x):
        y = canvas.y1 - self._H / 2 - self._PAD
        self._canvas = canvas
        self._hl = canvas.create_text((x, y), text='♥', fill='yellow')

    def update_y(self, y1):
        x, _ = self._canvas.coords(self._hl)
        y = y1 - self._H / 2 - self._PAD
        self._canvas.coords(self._hl, (x, y))

class _Z(tk.Canvas):

    _WPAD = 20
    _H = 50

    def __init__(self, master, media, nglw):
        tk.Canvas.__init__(self, master, bg='#222222')

        self._rect = None
        self._segments = []
        self._media = media
        self._nglw = nglw

        w, h = self.winfo_reqwidth(), self.winfo_reqheight()
        rect_coords = self._set_rect_coords(w, h)
        self._rect = self.create_rectangle(*rect_coords, fill='#333355')

        self._highlights = []
        for hl_pos in nglw.highlights:
            scaled_pos = hl_pos / media.duration # 0 -> 1
            x = scaled_pos * (self.x2 - self.x1)
            hl = _Highlight(self, hl_pos)
            self._highlights.append(hl)

        self.bind('<Configure>', self._configure)
        self.bind('<Button-1>', self._create_segment)
        self.bind('<Double-Button-1>', self._kill_segment)
        self.bind('<B1-Motion>', self._seek)

    def _seek(self, event):
        w = self.x2 - self.x1
        pos = max(min(event.x, w), 0) / float(w)
        self._nglw.seek(pos * _UNIT_BASE)

    def _set_rect_coords(self, w, h):
        self.x1 = self._WPAD
        self.y1 = (h - self._H) / 2.
        self.x2 = w - self._WPAD
        self.y2 = self.y1 + self._H
        return self.x1, self.y1, self.x2, self.y2

    def _configure(self, event):
        x1, y1, x2, y2 = self._set_rect_coords(event.width, event.height)
        self.coords(self._rect, x1, y1, x2, y2)
        for segment in self._segments:
            segment.update_y(y1, y2)
        for highlight in self._highlights:
            highlight.update_y(y1)

    def set_color(self, color):
        self._color = color

    def _get_seg_block(self, x, dist_func, segment_exclude):
        min_dist = None
        closest = None
        for segment in self._segments:
            if segment == segment_exclude:
                continue
            if segment.x1 <= x <= segment.x2:
                return segment
            dist = dist_func(x, segment)
            if dist > 0 and (min_dist is None or dist < min_dist):
                closest = segment
                min_dist = dist
        return closest

    def get_min_x(self, x, segment_exclude=None):
        dist_func = lambda x, seg: x - seg.x2
        seg = self._get_seg_block(x, dist_func, segment_exclude)
        return seg.x2 if seg else self.x1

    def get_max_x(self, x, segment_exclude=None):
        dist_func = lambda x, seg: seg.x1 - x
        seg = self._get_seg_block(x, dist_func, segment_exclude)
        return seg.x1 if seg else self.x2

    def get_segment_from_x(self, x):
        for segment in self._segments:
            if segment.x1 <= x <= segment.x2:
                return segment
        return None

    def _create_segment(self, event):
        x = event.x

        segment = self.get_segment_from_x(x)
        if segment:
            #print('clicked an existing segment, ignoring')
            return

        min_x = self.get_min_x(x)
        max_x = self.get_max_x(x)
        #print(f'min_x={min_x} max_x={max_x}')

        avail = max_x - min_x
        if avail < _Segment.W:
            #print(f'no space for segment (avail:{avail} need:{_Segment.W})')
            return

        x_start = x - _Segment.W / 2
        if x_start < min_x:
            x_start = min_x
        elif x_start + _Segment.W > max_x:
            x_start = max_x - _Segment.W

        #print(f'create segment at x={x_start}')
        segment = _Segment(self, x_start, self._color)
        self._segments.append(segment)

    def _kill_segment(self, event):
        x = event.x
        segment = self.get_segment_from_x(x)
        if segment:
            self._segments.remove(segment)
            segment.remove()


class _Buttons:

    _OK, _KO = range(2)

    def __init__(self, master, z):
        self._z = z
        btn_frame = ttk.Frame(master)
        self._ok_btn = tk.Button(btn_frame, text='☺', fg='white', command=self._set_ok, bg='green')
        self._ko_btn = tk.Button(btn_frame, text='☹', fg='white', command=self._set_ko, bg='red')
        self._ok_btn.pack(side=tk.LEFT)
        self._ko_btn.pack(side=tk.LEFT)
        btn_frame.pack()
        self._set_ok()

    def _set_ko(self):
        self._ok_btn.configure(relief='groove')
        self._ko_btn.configure(relief='sunken')
        self._z.set_color('red')

    def _set_ok(self):
        self._ok_btn.configure(relief='sunken')
        self._ko_btn.configure(relief='groove')
        self._z.set_color('green')


def  _main():
    import sys

    w = tk.Tk()
    w.title('Trim view')

    style = ttk.Style()
    style.theme_use('clam')

    media = Media(sys.argv[1])

    nglw = _NGLWidget(w, media)
    nglw.pack(fill=tk.BOTH, expand=tk.YES)

    seekbar = ttk.Scale(w, from_=0, to=_UNIT_BASE, command=nglw.seek, orient=tk.HORIZONTAL)
    seekbar.pack(fill=tk.X)

    z = _Z(w, media, nglw)
    buttons = _Buttons(w, z)

    z.pack(fill=tk.BOTH, expand=True)

    w.mainloop()

if __name__ == '__main__':
    _main()
