/*
A driver is just a special kind of kernel module that knows:
- how to talk to a particular piece of hardware, and
- how to translate user-space actions (like reading a file, or plugging in a USB) into hardware-level commands.
*/

// Refer learning.md or https://docs.kernel.org/driver-api/usb/usb.html

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

/* Step 1: Define which USB devices this driver supports */
static struct usb_device_id usbinfo_table[] = {
    { USB_DEVICE(0x0781, 0x5567) },  // SanDisk Cruzer Blade
    { }  // Terminating entry
};
MODULE_DEVICE_TABLE(usb, usbinfo_table);

/* Step 2: Called when matching device is plugged in */
static int my_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    struct usb_device *usbinfo = interface_to_usbdev(interface);  

    pr_info("SanDisk USB inserted!\n");

    if (usbinfo) {
        pr_info("Manufacturer: %s\n", usbinfo->manufacturer ? usbinfo->manufacturer : "Unknown");
        pr_info("Product: %s\n", usbinfo->product ? usbinfo->product : "Unknown");
        pr_info("Device speed enum: %d\n", usbinfo->speed);
    }

    return 0;  // success
}

/* Step 3: Called when device is unplugged */
static void driver_disconnect(struct usb_interface *interface)
{
    pr_info("SanDisk USB removed!\n");
}

/* Step 4: Bind everything together into usb_driver struct */
static struct usb_driver usb_driver_payload = {
    .name = "sandisk_driver",       // no spaces
    .id_table = usbinfo_table,
    .probe = my_probe,
    .disconnect = driver_disconnect,
};

/* Step 5: Register when module is loaded */
static int __init myusb_driver_init(void)
{
    pr_info("Initializing SanDisk USB driver module...\n");

    int result = usb_register_driver(&usb_driver_payload, THIS_MODULE, "sandisk_driver");
    if (result != 0) {
        pr_err("USB Monitor: usb_register failed. Error: %d\n", result);
        return result;
    }

    pr_info("USB Monitor: Module loaded successfully.\n");
    return 0;
}

/* Step 6: Unregister when module is removed */
static void __exit myusb_driver_exit(void)
{
    pr_info("USB driver unloading...\n");
usb_deregister(&usb_driver_payload);
    pr_info("USB Monitor: Module unloaded successfully.\n");
}

module_init(myusb_driver_init);
module_exit(myusb_driver_exit);

/* Module metadata */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aadityansha");
MODULE_DESCRIPTION("Simple USB kernel driver for SanDisk Cruzer Blade");
