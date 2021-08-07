#include "../../../../../include/core/p_render/backend/wsi/WindowSystem.hpp"

#include "../../../../../include/core/p_render/backend/Context.hpp"

#include "../../../../../include/core/PEngineCore.hpp"

using namespace Backend;

WindowSystem::WindowSystem(PEngine *core, Context *context) { // no reason to pass both of these tbh
    // should allocate all resources in the ctor for RAII

    // store any required inputs!
    core_ = core;
    context_ = context;

    // first allocate the data for the window system
    wsiData_ = std::make_unique<WindowSystemData>();
    // now pass a reference to the WSI data to be built up 

    // set up wsiData initially
    wsiData_->logicalDevice = context_->getLogicalDevice();
    wsiData_->presentationSurface = context_->getPresentationSurface();
    wsiData_->selectedPhysicalDevice = context_->getSelectedPhysicalDevice();

    wsiData_->win32Instance = core_->getHINSTANCE();
    wsiData_->mainWindow = core_->getMainWindowHWND();
    
    // setup swapchain
    setupSwapchain(*wsiData_);
    
    // finally fill the swapchain indices vector
    wsiData_->swapchainImageIndices.clear();
    for (uint32_t i = 0; i < wsiData_->numSwapchainImages; i++) {
        wsiData_->swapchainImageIndices.push_back(i);
    }
}

// WindowSystem::~WindowSystem() {
//     // should destroy all relevant data here, following RAII as always
//     // i think i can just make a dtor for WindowSystemData that cleans it up 
// }

void WindowSystem::createPresentationSurface(WindowSystemData &wsiData) {

    // #ifdef _WIN32
    // // auto wos = std::dynamic_pointer_cast<WindowsInterface>(os_);
    // // WindowData windowData = {
    // //     wos->getHINSTANCE(),
    // //     wos->getMainWindowHWND()
    // // };
    // // windowData.hinst =
    // // windowData.hwnd =  wos->getMainWindowHWND();

    // // create win32 surface 
    // VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfo = {
    //     VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
    //     nullptr,
    //     0,
    //     GetModuleHandle(NULL), // just get the current hinstance through win32 api
    //     wsiData_->mainWindow
    // };
    // // win32SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    // // win32SurfaceCreateInfo.pNext = nullptr;
    // // win32SurfaceCreateInfo.flags = 0;
    // // win32SurfaceCreateInfo.hinstance = GetModuleHandle(NULL);
    // // win32SurfaceCreateInfo.hwnd = windowData.hwnd; 

    // wsiData.presentationSurface = VK_NULL_HANDLE;
    // if (vkCreateWin32SurfaceKHR(context_->getInstance(), &win32SurfaceCreateInfo, nullptr, &wsiData.presentationSurface) != VK_SUCCESS) {
    //     throw std::runtime_error("unable to create presentation surface :(");
    //     exit(1);
    // }

    // if (wsiData.presentationSurface == VK_NULL_HANDLE) {
    //     throw std::runtime_error("unable to create presentation surface :(");
    //     exit(1);
    // }
    // #endif
}

void WindowSystem::setSwapchainImageUses(WindowSystemData &wsiData, VkSurfaceCapabilitiesKHR &capabilities) {
    wsiData.desiredImageUsages = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // now have to check against the possible image usages
    VkImageUsageFlags potentialImageUsages = wsiData.desiredImageUsages & capabilities.supportedUsageFlags;
    if (potentialImageUsages != wsiData.desiredImageUsages) {
        throw std::runtime_error("desired swapchain image usage not available!");
        exit(1);
    }
    else {
        wsiData.imageUsages = wsiData.desiredImageUsages;
    }
}

void WindowSystem::setSwapchainImageTransform(WindowSystemData &wsiData, VkSurfaceCapabilitiesKHR &capabilities) {
    // set the swapchain transform used by the engine
        // not entirely sure what this has to be set to, but it shouldn't be too bad
    // just trying this for now

    //FEB17 - i think both the transform and image usages function have to actually do what they say they do... 
    wsiData.desiredSurfaceTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

    if (wsiData.desiredSurfaceTransform & capabilities.supportedTransforms) {
        wsiData.surfaceTransform = wsiData.desiredSurfaceTransform;
    }
    else {
        throw std::runtime_error("desired surface transform unavailable!"); // shouldn't happen
        exit(1);
    }
}

void WindowSystem::setSwapchainImageFormat(WindowSystemData &wsiData) {
    // set the swapchain image format here
        // not entirely sure what needs to be done, gotta read

    // i think we're setting 3 things:
        // 1. VkFormat -> image format (components, precision, data type of an image's pixels)
        // 2. VkColorSpaceKHR -> choose the color space for the engine (how colors are encoded for hardware)
        // 3. VkSurfaceFormatKHR -> a pairing of 1 and 2 
    
    /* SET DESIRED FORMAT */
    // gonna try these for now 
    // instance.desiredSurfaceFormat.format = VK_FORMAT_R8G8B8A8_UNORM; //FEB17- found the problem maybe: incorrect surface format!
    wsiData.desiredSurfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
    wsiData.desiredSurfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    // FEB17 - logical device creation is working, but calling vkGetPhysicalDeviceSurfaceFormatsKHR causes the error spam now
        // might have to try moving the below to later, and use this function just for setting up the desired surface format
        // for now, gonnna just see if my changes so far help

    // get formats using selected physical device
    uint32_t formatsCount;
    auto result = vkGetPhysicalDeviceSurfaceFormatsKHR(context_->getSelectedPhysicalDevice(), wsiData.presentationSurface, &formatsCount, nullptr);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("unable to acquire physical device surface formats!");
        exit(1);
    }

    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatsCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(context_->getSelectedPhysicalDevice(), wsiData.presentationSurface, &formatsCount, surfaceFormats.data()); 
    if (result != VK_SUCCESS) {
        throw std::runtime_error("jeanine restrepo didn't want this to happen... so it didn't");
        exit(1);
    }

    // first check if surfaceFormats has only one element equal to VK_FORMAT_UNDEFINED, which means we can use anything for the surface format
    if ((surfaceFormats.size() == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED)) {
        // set the format as we want?
        wsiData.surfaceFormat = wsiData.desiredSurfaceFormat;
        return;
    }

    // else we have to set the format this way:
    bool foundFormat = false;
    for (const auto &format : surfaceFormats) {
        if ((format.format == wsiData.desiredSurfaceFormat.format) && (format.colorSpace == wsiData.desiredSurfaceFormat.colorSpace)) {
            wsiData.surfaceFormat.format = wsiData.desiredSurfaceFormat.format;
            wsiData.surfaceFormat.colorSpace = wsiData.desiredSurfaceFormat.colorSpace;
            foundFormat = true; 
            break;
        }
    }

    if (!foundFormat) {
        // just set it to tthe first available format...
        std::cout << "Unable to find desired surface format :(" << std::endl;
        wsiData.surfaceFormat.format = surfaceFormats[0].format;
        wsiData.surfaceFormat.colorSpace = surfaceFormats[0].colorSpace;
    }
}

void WindowSystem::setupSwapchain(WindowSystemData &wsiData) {
    // REWRITE 
        // gonna try and just get the surface present capabilities once and pass it in
    setSurfacePresentCapabilities(wsiData);
    setPresentationMode(wsiData);

    // should probably just recreate the presentation surface here if it's lost at this point...
    struct WindowData {
        HINSTANCE hinst;
        HWND hwnd;
    } windowData;

    windowData.hinst = core_->getHINSTANCE();
    windowData.hwnd = core_->getMainWindowHWND();

    VkSurfaceCapabilitiesKHR capabilities;
    auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context_->getSelectedPhysicalDevice(), wsiData.presentationSurface, &capabilities);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("unable to get physical device surface capabilities!");
        exit(1);
    }

    setSwapchainImageUses(wsiData, capabilities);
    setSwapchainImageTransform(wsiData, capabilities);
    setSwapchainImageFormat(wsiData);
    // now create the swapchain
        // recall this is what is actually used to display images on the screen, so it's pretty fundamental

    // hmm i think i need to set up the indices
    for (uint32_t i = 0; i < wsiData.numSwapchainImages; i++) {
        wsiData.swapchainImageIndices.push_back(i);
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo;
    VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE;
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.pNext = nullptr;
    swapchainCreateInfo.flags = 0;
    swapchainCreateInfo.surface = wsiData.presentationSurface;
    swapchainCreateInfo.minImageCount = wsiData.numSwapchainImages;
    swapchainCreateInfo.imageFormat = wsiData.surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = wsiData.surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = wsiData.swapchainImageSize;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = wsiData.imageUsages;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0; 
    swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    swapchainCreateInfo.preTransform = wsiData.surfaceTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = wsiData.presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = oldSwapchain;

    result = vkCreateSwapchainKHR(wsiData.logicalDevice, &swapchainCreateInfo, nullptr, &wsiData.swapchain);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("unable to create swapchain :(");
        exit(1);
    }

    // after creating swapchain we'll create the semaphores
    createSwapchainImageSemaphores(wsiData);

    // after creating the swapchain, have to get the image handles 
    getSwapchainImageHandles(wsiData);

    getSwapchainImageViews(wsiData);
}

void WindowSystem::setPresentationMode(WindowSystemData &wsiData) {
    // ideally we want to use triple buffering I think, which in Vulkan you can accomplish by using the MAILBOX present mode with
    // an extra image being used as the third buffer
        // provides a good balance between less input lag and less screen tearing
        // a way to limit FPS would also accomplish this 
    // either way that's just the default, i need to probably write a whole other submodule/more interface functions for configuring Vulkan to
    // accomodate modifiable graphics options in the game

    uint32_t presentationModesCount = 0;
    VkResult result = VK_SUCCESS;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(wsiData.selectedPhysicalDevice, wsiData.presentationSurface, &presentationModesCount, nullptr);
    if ((result != VK_SUCCESS) || (presentationModesCount == 0)) {
        throw std::runtime_error("unable to get physical device surface present modes!");
        exit(1);
    }

    std::vector<VkPresentModeKHR> presentationModes(presentationModesCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(wsiData.selectedPhysicalDevice, wsiData.presentationSurface, &presentationModesCount, presentationModes.data());
    if ((result != VK_SUCCESS) || (presentationModesCount == 0)) {
        throw std::runtime_error("unable to get physical device surface present modes!");
        exit(1);
    }

    // cool lol
    bool mailboxFound = false;
    for (const auto &presentMode : presentationModes) {
        // look for mailbox present mode
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            mailboxFound = true;
            break;
        }
    }

    // set the present mode 
    // bool mailboxFound = true; // for now
    if (mailboxFound) {
        // set it to MAILBOX
        wsiData.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    }
    else {
        // set it to FIFO 
        wsiData.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    }
}

void WindowSystem::setSurfacePresentCapabilities(WindowSystemData &wsiData) {
    VkSurfaceCapabilitiesKHR capabilities;
    auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context_->getSelectedPhysicalDevice(), wsiData.presentationSurface, &capabilities);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("unable to get physical device surface capabilities!");
        exit(1);
    }

    wsiData.numSwapchainImages = (wsiData.presentMode == VK_PRESENT_MODE_MAILBOX_KHR) ? 3 : capabilities.minImageCount + 1; // either triple buffer or just use minimagecount...
    if (capabilities.maxImageCount > 0) {
        if (wsiData.numSwapchainImages > capabilities.maxImageCount) {
            wsiData.numSwapchainImages = capabilities.maxImageCount;
        }
    }

    if ((capabilities.currentExtent.width == 0xFFFFFFFF) || (capabilities.currentExtent.height == 0xFFFFFFFF)) {
        // here have to set the swapchain size differently... not sure tho, might mostly skip it for now
        wsiData.swapchainImageSize.width = std::clamp<uint32_t>(capabilities.currentExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        wsiData.swapchainImageSize.height = std::clamp<uint32_t>(capabilities.currentExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }
    else {
        wsiData.swapchainImageSize = capabilities.currentExtent;
    }
}

void WindowSystem::getSwapchainImageHandles(WindowSystemData &wsiData) {
    // get swapchain image handles
    wsiData.swapchainImages.resize(wsiData.numSwapchainImages);

    // also size the imageAcquired/rendering semaphores vector
    // instance.swapchainImageAcquiredSemaphores.resize(instance.numSwapchainImages);
   // instance.swapchainRenderingCompleteSemaphores.resize(instance.numSwapchainImages);
        // APR9 - skipping this for now, using a simpler synchronization method to start 

    uint32_t swapchainImageCount = 0;
    auto result = vkGetSwapchainImagesKHR(wsiData.logicalDevice, wsiData.swapchain, &swapchainImageCount, nullptr);
    if ((result != VK_SUCCESS) || (swapchainImageCount == 0)) {
        throw std::runtime_error("unable to get swapchain image handles...");
    }

    result = vkGetSwapchainImagesKHR(wsiData.logicalDevice, wsiData.swapchain, &swapchainImageCount, wsiData.swapchainImages.data());
    if ((result != VK_SUCCESS) || (swapchainImageCount == 0)) {
        throw std::runtime_error("unable to get swapchain image handles...");
    }

}

void WindowSystem::createSwapchainImageSemaphores(WindowSystemData &wsiData) {
    
    // VkSemaphoreCreateInfo genericCreateInfo = {};
    // genericCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    // genericCreateInfo.flags = 0; // i think we can just have this be 0
    // genericCreateInfo.pNext = nullptr;

    // VkResult result;
    // for (int i = 0; i < wsiData.swapchainImageAcquiredSemaphores.size(); i++) {
    //     result = vkCreateSemaphore(wsiData.logicalDevice, &genericCreateInfo, nullptr, &wsiData.swapchainImageAcquiredSemaphores[i]);
    //     if (result != VK_SUCCESS) {
    //         throw std::runtime_error("Unable to create SwapchainImageAcquired semaphore!");
    //     }
    // }

    // for (int j = 0; j < wsiData.swapchainRenderCompleteSemaphores.size(); j++) {
    //     result = vkCreateSemaphore(wsiData.logicalDevice, &genericCreateInfo, nullptr, &wsiData.swapchainRenderCompleteSemaphores[j]);
    //     if (result != VK_SUCCESS) {
    //         throw std::runtime_error("Unable to create SwapchainImageAcquired semaphore!");
    //     }
    // }

    VkSemaphoreCreateInfo semaphoreInfo = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    
    auto result = vkCreateSemaphore(context_->getLogicalDevice(), &semaphoreInfo, nullptr, &wsiData_->swapchainRenderCompleteSemaphore);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Unable to create semaphore:(");
    }

    result = vkCreateSemaphore(context_->getLogicalDevice(), &semaphoreInfo, nullptr, &wsiData_->swapchainPresentSemaphore);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Unable to create semaphore:(");
    }

    // APR9 - skipping these for now
}

void WindowSystem::getSwapchainImageViews(WindowSystemData &wsiData) {
    assert(wsiData.numSwapchainImages > 0);
    for (const auto &swapchainImage : wsiData.swapchainImages) {
        // create an image view for this swapchain image
        VkImageViewCreateInfo imageViewCreateInfo;
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = nullptr;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.image = swapchainImage;

        // APR6 - i think i'm missing some stuff?
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        imageViewCreateInfo.format = wsiData.surfaceFormat.format;
        
        // i believe the component map is for swizzling which will not be used rn
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // set the subresource ranges...
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // good golly i gotta think about how to organize swapchain stuff better
        imageViewCreateInfo.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS; // dont know why you use this, but it's kool for now
        imageViewCreateInfo.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0; // these base levels are set to 0 which i guess makes sense
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;

        VkImageView newImageView;
        auto result = vkCreateImageView(wsiData.logicalDevice, &imageViewCreateInfo, nullptr, &newImageView);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("unable to create swapchain image view!");
            exit(1);
        }

        wsiData.swapchainImageViews.push_back(newImageView);
    }
}