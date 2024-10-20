#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/version.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/drivers/gpio.h>
#include <ctype.h>
#include <strings.h>

#ifdef CONFIG_ARCH_POSIX
#include <unistd.h>
#else
#include <zephyr/posix/unistd.h>
#endif

LOG_MODULE_REGISTER(app_gpio);

static const struct device *get_gpio_device_from_port(const char *port)
{
	if (strncasecmp(port, "A", 1) == 0) {
		return DEVICE_DT_GET(DT_NODELABEL(gpioa)); // GPIO Port A
	} else if (strncasecmp(port, "B", 1) == 0) {
		return DEVICE_DT_GET(DT_NODELABEL(gpiob)); // GPIO Port B
	} else if (strncasecmp(port, "C", 1) == 0) {
		return DEVICE_DT_GET(DT_NODELABEL(gpioc)); // GPIO Port C
	} else if (strncasecmp(port, "D", 1) == 0) {
		return DEVICE_DT_GET(DT_NODELABEL(gpiod)); // GPIO Port D
	} else if (strncasecmp(port, "E", 1) == 0) {
		return DEVICE_DT_GET(DT_NODELABEL(gpioe)); // GPIO Port E
	} else if (strncasecmp(port, "F", 1) == 0) {
		return DEVICE_DT_GET(DT_NODELABEL(gpiof)); // GPIO Port F
	} else if (strncasecmp(port, "G", 1) == 0) {
		return DEVICE_DT_GET(DT_NODELABEL(gpiog)); // GPIO Port G
	} else if (strncasecmp(port, "H", 1) == 0) {
		return DEVICE_DT_GET(DT_NODELABEL(gpioh)); // GPIO Port H
	} else if (strncasecmp(port, "I", 1) == 0) {
		return DEVICE_DT_GET(DT_NODELABEL(gpioi)); // GPIO Port I
	} else if (strncasecmp(port, "J", 1) == 0) {
		return DEVICE_DT_GET(DT_NODELABEL(gpioj)); // GPIO Port J
	}
	return NULL; // Invalid port
}
/* Command: Set GPIO Pin Direction (Input or Output) */
static int cmd_gpio_dir(const struct shell *shell, size_t argc, char **argv)
{
	if (argc != 4) {
		shell_print(shell, "Usage: gpio_dir <port> <pin> <input/output>");
		return -EINVAL;
	}

	const char *port = argv[1];
	int pin = atoi(argv[2]);
	const char *dir = argv[3];

	const struct device *gpio_dev = get_gpio_device_from_port(port);

	if (!gpio_dev) {
		shell_print(shell, "Invalid port: %s", port);
		return -EINVAL;
	}

	int ret;
	if (strcmp(dir, "input") == 0) {
		ret = gpio_pin_configure(gpio_dev, pin, GPIO_INPUT);
	} else if (strcmp(dir, "output") == 0) {
		ret = gpio_pin_configure(gpio_dev, pin, GPIO_OUTPUT);
	} else {
		shell_print(shell, "Invalid direction. Use 'input' or 'output'");
		return -EINVAL;
	}

	if (ret < 0) {
		shell_print(shell, "Failed to configure pin %d on port %s", pin, port);
		return ret;
	}

	shell_print(shell, "Port %s, Pin %d configured as %s", port, pin, dir);
	return 0;
}

/* Command: Set GPIO Pin State */
static int cmd_gpio_set(const struct shell *shell, size_t argc, char **argv)
{
	if (argc != 4) {
		shell_print(shell, "Usage: gpio_set <port> <pin> <0/1>");
		return -EINVAL;
	}

	const char *port = argv[1];
	int pin = atoi(argv[2]);
	int value = atoi(argv[3]);

	const struct device *gpio_dev = get_gpio_device_from_port(port);

	if (!gpio_dev) {
		shell_print(shell, "Invalid port: %s", port);
		return -EINVAL;
	}

	if (value != 0 && value != 1) {
		shell_print(shell, "Invalid value. Use 0 or 1.");
		return -EINVAL;
	}

	int ret = gpio_pin_set(gpio_dev, pin, value);
	if (ret < 0) {
		shell_print(shell, "Failed to set pin %d on port %s", pin, port);
		return ret;
	}

	shell_print(shell, "Port %s, Pin %d set to %d", port, pin, value);
	return 0;
}

/* Command: Get GPIO Pin State */
static int cmd_gpio_get(const struct shell *shell, size_t argc, char **argv)
{
	if (argc != 3) {
		shell_print(shell, "Usage: gpio_get <port> <pin>");
		return -EINVAL;
	}

	const char *port = argv[1];
	int pin = atoi(argv[2]);

	const struct device *gpio_dev = get_gpio_device_from_port(port);

	if (!gpio_dev) {
		shell_print(shell, "Invalid port: %s", port);
		return -EINVAL;
	}

	int value = gpio_pin_get(gpio_dev, pin);
	if (value < 0) {
		shell_print(shell, "Failed to get state of pin %d on port %s", pin, port);
		return value;
	}

	shell_print(shell, "Port %s, Pin %d is %d", port, pin, value);
	return 0;
}

/* GPIO Commands */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_gpio,
			       SHELL_CMD(dir, NULL, "Set GPIO pin direction (input/output)",
					 cmd_gpio_dir),
			       SHELL_CMD(set, NULL, "Set GPIO pin value (0/1)", cmd_gpio_set),
			       SHELL_CMD(get, NULL, "Get GPIO pin value", cmd_gpio_get),
			       SHELL_SUBCMD_SET_END /* Asrray terminated. */
);
SHELL_CMD_REGISTER(gpio, &sub_gpio, "GPIO commands", NULL);