#include "../../../../../include/core/p_render/render_graph/resources/ImageResource.hpp"

// #include "../../../../../include/core/p_render/render_graph/resources/ImageView.hpp"

ImageResource::ImageResource(unsigned int index, const std::string &name, const AttachmentInfo &info) : RenderResource(RenderResource::Type::Image, index, name) {
    _attachmentInfo = info;
}

// this is mostly implemented in the header (inconsistent)