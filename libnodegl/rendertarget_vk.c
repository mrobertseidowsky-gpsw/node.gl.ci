#include <stdlib.h>
#include <vulkan/vulkan_core.h>

#include "format.h"
#include "log.h"
#include "memory.h"
#include "nodes.h"
#include "rendertarget.h"

static int is_depth_attachment(VkFormat format)
{
    switch (format) {
    case VK_FORMAT_D16_UNORM:
    case VK_FORMAT_D16_UNORM_S8_UINT:
    case VK_FORMAT_D24_UNORM_S8_UINT:
    case VK_FORMAT_D32_SFLOAT:
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
        return -1;
    default:
        return 0;
    }
}

int ngli_rendertarget_init(struct rendertarget *s, struct ngl_ctx *ctx, const struct rendertarget_params *params)
{
    int ret = -1;
    struct glcontext *vk = ctx->glcontext;

    s->ctx = ctx;
    s->width = params->width;
    s->height = params->height;

    VkAttachmentDescription *attachment_descriptions = ngli_calloc(params->nb_attachments, sizeof(*attachment_descriptions));
    if (!attachment_descriptions)
        return -1;

    VkAttachmentReference *color_attachments = ngli_calloc(params->nb_attachments, sizeof(*color_attachments));
    if (!color_attachments)
        return -1;
    int nb_color_attachments = 0;
    int nb_depth_attachments = 0;
    VkAttachmentReference depth_attachment = {0};

    for (int i = 0; i < params->nb_attachments; i++) {
        const struct texture *texture = params->attachments[i];
        VkAttachmentDescription *desc = &attachment_descriptions[i];
        VkFormat format;
        ngli_format_get_vk_format(vk, texture->params.format, &format);
        desc->format = format;
        desc->samples = VK_SAMPLE_COUNT_1_BIT;
        desc->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        desc->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        desc->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        desc->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        desc->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        desc->finalLayout = VK_IMAGE_LAYOUT_GENERAL;

        if (is_depth_attachment(texture->format)) {
            depth_attachment.attachment = i;
            depth_attachment.layout = texture->image_layout;
            nb_depth_attachments++;
        } else {
            color_attachments[nb_color_attachments].attachment = i;
            color_attachments[nb_color_attachments].layout = texture->image_layout;
            nb_color_attachments++;
        }
    }

    VkSubpassDescription subpass_description = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = nb_color_attachments,
        .pColorAttachments = color_attachments,
    };

    if (nb_depth_attachments > 0)
        subpass_description.pDepthStencilAttachment = &depth_attachment;

    VkSubpassDependency dependencies[2] = {0};
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo render_pass_create_info = {};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.attachmentCount = params->nb_attachments;
    render_pass_create_info.pAttachments = attachment_descriptions;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass_description;
    render_pass_create_info.dependencyCount = NGLI_ARRAY_NB(dependencies);
    render_pass_create_info.pDependencies = dependencies;

    VkResult res = vkCreateRenderPass(vk->device, &render_pass_create_info, NULL, &s->render_pass);
    if (res != VK_SUCCESS)
        return -1;
    s->render_area = (VkExtent2D){s->width, s->height};

    VkImageView *attachments = ngli_calloc(params->nb_attachments, sizeof(*attachments));
    if (!attachments)
        return -1;
    LOG(ERROR, "=%d", params->nb_attachments);
    for (int i = 0; i < params->nb_attachments; i++) {
        struct texture *texture = params->attachments[i];
        attachments[i] = texture->image_view;
        LOG(ERROR, "i=%d %p", i, texture->image_view);
    }

    VkFramebufferCreateInfo framebuffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = s->render_pass,
        .attachmentCount = params->nb_attachments,
        .pAttachments = attachments,
        .width = s->width,
        .height = s->height,
        .layers = 1
    };


    res = vkCreateFramebuffer(vk->device, &framebuffer_create_info, NULL, &s->framebuffer);
    if (res != VK_SUCCESS)
        return -1;

    return 0;
}

void ngli_rendertarget_blit(struct rendertarget *s, struct rendertarget *dst, int vflip)
{
    LOG(ERROR, "stub");
}

void ngli_rendertarget_read_pixels(struct rendertarget *s, uint8_t *data)
{
    LOG(ERROR, "stub");
}

void ngli_rendertarget_reset(struct rendertarget *s)
{
    if (!s->ctx)
        return;

    struct glcontext *vk = s->ctx->glcontext;
    vkDestroyRenderPass(vk->device, s->render_pass, NULL);
    vkDestroyFramebuffer(vk->device, s->framebuffer, NULL);
}
