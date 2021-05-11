#include <stdio.h> /*For print and the like*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#define DEVICE_INDEX 0

SDL_Window*                g_sdl_window;

int                        g_i_window_width = 1600;
int                        g_i_window_height= 900;

VkApplicationInfo          g_vk_app_info;
VkInstanceCreateInfo       g_vk_inst_create_info;
VkInstance                 g_vk_instance;

int main(){
	unsigned int l_i_count = 0;
	unsigned int l_i_layer_count = 0;
	unsigned int l_i_device_ext_count = 0;
	unsigned int iter = 0;
	SDL_Event l_sdl_event;

	VkExtensionProperties*      l_vkextensionprops;
	VkLayerProperties*          l_vklayerprops;
	VkResult                    l_vkresult;
	VkPhysicalDevice*           l_vkdevices;
	VkPhysicalDeviceProperties* l_vkdeviceprops;
	VkQueueFamilyProperties*    l_vkqueuefamilyprops;
	VkDevice                    l_vkdevice;
	VkDeviceQueueCreateInfo     l_vkdevicequeueinfo;
	VkDeviceCreateInfo          l_vkdevicecreateinfo;
	VkExtensionProperties*      l_vkdeviceextensionprops;
	VkQueue                     l_graphics_queue;
	
	unsigned int               l_qfamilyindex;
	
	const char** l_cpp_sdl_extensions = 0;
	const char** l_cpp_sdl_layers = 0;
	const char** l_cpp_dev_ext_names = 0;
	float         l_fa_priorities[1] = {0.0f};

	printf("\n\t\tPure C vulkan test\n\n");

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0){
		printf("Failed to initialize SDL\n");
		return 0;
	}	
	
	g_sdl_window = SDL_CreateWindow("C Vulkan Test",
									SDL_WINDOWPOS_CENTERED,
									SDL_WINDOWPOS_CENTERED,
									g_i_window_width,
									g_i_window_height,
									SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

	if(!g_sdl_window){
		printf("Failed to create SDL window");
		return 0;
	}
	
	if(vkEnumerateInstanceExtensionProperties(0, &l_i_count, 0) != VK_SUCCESS){
		printf("Failed to enumerate vulkan extensions\n");
		return 0;
	}

	printf("Found %i vulkan extensions\n",l_i_count);

	l_vkextensionprops = malloc(sizeof(VkExtensionProperties)*l_i_count);

	vkEnumerateInstanceExtensionProperties(0, &l_i_count,l_vkextensionprops );

	for(iter = 0; iter < l_i_count; ++iter)
		printf("\t%s\n",l_vkextensionprops[iter].extensionName);

	printf("\n");

	g_vk_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	g_vk_app_info.pApplicationName = "Based Vulkan Test";
	g_vk_app_info.applicationVersion = VK_MAKE_VERSION(1,0,0);
	g_vk_app_info.pEngineName = "hk";
	g_vk_app_info.engineVersion = VK_MAKE_VERSION(1,0,0);
	g_vk_app_info.apiVersion = VK_API_VERSION_1_0;
	
	g_vk_inst_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	g_vk_inst_create_info.pApplicationInfo = &g_vk_app_info;
	
	l_i_count = 0;


	if(SDL_Vulkan_GetInstanceExtensions(g_sdl_window,&l_i_count,0) == 0){
		printf("Failed to get Vulkan extensions from SDL");
		return 0;
	}

	/* For some reason in the vkQuake code it was   l_i_count+3*/
	l_cpp_sdl_extensions = malloc(sizeof(const char*) * (l_i_count));

	SDL_Vulkan_GetInstanceExtensions(g_sdl_window,&l_i_count,l_cpp_sdl_extensions); 

	printf("Found %i required vulkan extensions for SDL\n",l_i_count);
	for(iter = 0; iter < l_i_count; ++iter)
		printf("\t%s\n",l_cpp_sdl_extensions[iter]);

	printf("\n");
	
	g_vk_inst_create_info.enabledExtensionCount = l_i_count;
	g_vk_inst_create_info.ppEnabledExtensionNames = l_cpp_sdl_extensions;

	l_i_count = 0;	

	if(vkEnumerateInstanceLayerProperties(&l_i_layer_count,0) != VK_SUCCESS){
		printf("Failed to get Vulkan layers\n");
		return 0;
	}	
	
	printf("Found %i vulkan layers\n",l_i_layer_count);

	l_vklayerprops = malloc(sizeof(VkLayerProperties)*l_i_layer_count);
	l_cpp_sdl_layers = malloc(sizeof(char*) * l_i_layer_count);
	vkEnumerateInstanceLayerProperties(&l_i_layer_count,l_vklayerprops);

	for(iter = 0; iter < l_i_count; ++iter){
		l_cpp_sdl_layers[iter] = l_vklayerprops[iter].layerName;
		printf("\t%s\n",l_cpp_sdl_layers[iter]);
	}
	printf("\n");

	g_vk_inst_create_info.enabledLayerCount = l_i_count;
	g_vk_inst_create_info.ppEnabledLayerNames = l_cpp_sdl_layers;

	l_vkresult = vkCreateInstance(&g_vk_inst_create_info,0,&g_vk_instance);	

	l_i_count = 0;
	if(vkEnumeratePhysicalDevices(g_vk_instance,&l_i_count,0) != VK_SUCCESS){
		printf("Failed to enumerate physical devices\n\n");
		return 0;
	}

	l_vkdevices = malloc(sizeof(VkPhysicalDevice)*l_i_count);
	vkEnumeratePhysicalDevices(g_vk_instance,&l_i_count,l_vkdevices);

	l_vkdeviceprops = malloc(sizeof(VkPhysicalDeviceProperties)*l_i_count);
	
	printf("\nListing GPUs\n");
	for(iter = 0; iter < l_i_count; ++iter){
		vkGetPhysicalDeviceProperties(l_vkdevices[iter],&l_vkdeviceprops[iter]);
		printf("\t%s - device %i\n",l_vkdeviceprops[iter].deviceName, iter);
	}	
	printf("\n\n");
	
	l_i_count = 0;
	/* Primary GPU */
	vkGetPhysicalDeviceQueueFamilyProperties(l_vkdevices[DEVICE_INDEX],&l_i_count,0);
	printf("Found %i Queue Family Properties on device %i\n",l_i_count, DEVICE_INDEX);
	
	l_vkqueuefamilyprops = malloc(sizeof(VkQueueFamilyProperties)*l_i_count);
	vkGetPhysicalDeviceQueueFamilyProperties(l_vkdevices[DEVICE_INDEX],&l_i_count,l_vkqueuefamilyprops);
	
	for(iter = 0; iter < l_i_count; ++iter){
		if(l_vkqueuefamilyprops[iter].queueCount > 0 && l_vkqueuefamilyprops[iter].queueFlags & VK_QUEUE_GRAPHICS_BIT){
			printf("Found VK_QUEUE_GRAPHICS_BIT capibilities on family index %i\n",iter);
				printf("\tqueueCount=%i\n\n",l_vkqueuefamilyprops[iter].queueCount);
			l_qfamilyindex = iter;
			break;
		}		
	}	
	if(vkEnumerateDeviceExtensionProperties(l_vkdevices[DEVICE_INDEX],0,&l_i_device_ext_count,0) != VK_SUCCESS){
		printf("Failed to get device extension properties\n");
		return 0;
	}
	
	l_vkdeviceextensionprops = malloc(sizeof(VkExtensionProperties)*l_i_device_ext_count);
	l_cpp_dev_ext_names = malloc(sizeof(char*)*l_i_device_ext_count);
	
	vkEnumerateDeviceExtensionProperties(l_vkdevices[DEVICE_INDEX],0,&l_i_device_ext_count,l_vkdeviceextensionprops);
	
	printf("Printing device %i extension properties \n",DEVICE_INDEX);
	for(iter = 0; iter < l_i_device_ext_count; ++iter){
		l_cpp_dev_ext_names[iter] = l_vkdeviceextensionprops[iter].extensionName;
		printf("\t\t%s",l_cpp_dev_ext_names[iter]);
		if(((iter+1) % 2) == 0)
			printf("\n");
	}
	printf("\n\n");

	l_vkdevicequeueinfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	l_vkdevicequeueinfo.queueFamilyIndex = l_qfamilyindex;
	l_vkdevicequeueinfo.queueCount = 1;
	l_vkdevicequeueinfo.pQueuePriorities = l_fa_priorities;
	l_vkdevicequeueinfo.pNext = 0;
	l_vkdevicequeueinfo.flags = 0;

	l_vkdevicecreateinfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	l_vkdevicecreateinfo.queueCreateInfoCount = 1;
	l_vkdevicecreateinfo.pQueueCreateInfos = &l_vkdevicequeueinfo;
	l_vkdevicecreateinfo.ppEnabledLayerNames = l_cpp_sdl_layers;
	l_vkdevicecreateinfo.enabledLayerCount = l_i_layer_count;
 	l_vkdevicecreateinfo.ppEnabledExtensionNames = l_cpp_dev_ext_names;
	l_vkdevicecreateinfo.enabledExtensionCount = l_i_device_ext_count;
	l_vkdevicecreateinfo.pNext = 0;
	l_vkdevicecreateinfo.pEnabledFeatures = 0;
	l_vkdevicecreateinfo.flags = 0;

	if(vkCreateDevice(l_vkdevices[DEVICE_INDEX], &l_vkdevicecreateinfo, 0, &l_vkdevice) != VK_SUCCESS){
		printf("Failed to create logical device\n\n");
	}

	vkGetDeviceQueue(l_vkdevice,l_qfamilyindex,0,&l_graphics_queue);

	do{

		SDL_PollEvent(&l_sdl_event);	

	}while(l_sdl_event.type != SDL_QUIT);

}
