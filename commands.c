#include <stdlib.h>
#include "microcom.h"

static int cmd_speed(int argc, char *argv[])
{
	int speed;
	speed_t flag;

	if (argc < 2) {
		printf("current speed: %d\n", current_speed);
		return 0;
	}

	speed = strtoul(argv[1], NULL, 0);
	flag = baudrate_to_flag(speed);
	if (flag < 0) {
		printf("invalid speed %d\n", speed);
		return 1;
	}

	current_speed = speed;
	ios->set_speed(ios, flag);

	return 0;
}

static int cmd_flow(int argc, char *argv[])
{
	char *flow;

	if (argc < 2) {
		switch (current_flow) {
		default:
		case FLOW_NONE:
			flow = "none";
			break;
		case FLOW_SOFT:
			flow = "soft";
			break;
		case FLOW_HARD:
			flow = "hard";
			break;
		}
		printf("current flow: %s\n", flow);
		return 0;
	}

	switch (*argv[1]) {
	case 'n':
		current_flow = FLOW_NONE;
		break;
	case 's':
		current_flow = FLOW_SOFT;
		break;
	case 'h':
		current_flow = FLOW_HARD;
		break;
	default:
		printf("unknown flow type \"%s\"\n", argv[1]);
		return 1;
	}

	ios->set_flow(ios, current_flow);

	return 0;
}

static int cmd_exit(int argc, char *argv[])
{
	return MICROCOM_CMD_START;
}

static int cmd_break(int argc, char *argv[])
{
	ios->send_break(ios);
	return 0;
}

static int cmd_quit(int argc, char *argv[])
{
	microcom_exit(0);
	return 0;
}

static int cmd_help(int argc, char *argv[])
{
	struct cmd *cmd;

	if (argc == 1) {
		for_each_command(cmd) {
			if (cmd->info)
				printf("%s - %s\n", cmd->name, cmd->info);
			else
				printf("%s\n", cmd->name);
		}
	} else {
		microcom_cmd_usage(argv[1]);
	}

	return 0;
}

static int cmd_execute(int argc, char *argv[])
{
	if (argc < 2)
		return MICROCOM_CMD_USAGE;

	return do_script(argv[1]);
}

static int cmd_comment(int argc, char *argv[])
{
	return 0;
}

static struct cmd cmds[] = {
	{
		.name = "speed",
		.fn = cmd_speed,
		.info = "set terminal speed",
		.help = "speed <newspeed>"
	}, {
		.name = "exit",
		.fn = cmd_exit,
		.info = "exit from command processing",
	}, {
		.name = "flow",
		.fn = cmd_flow,
		.info = "set flow control",
		.help = "flow hard|soft|none",
	}, {
		.name = "break",
		.fn = cmd_break,
		.info = "send break",
	}, {
		.name = "quit",
		.fn = cmd_quit,
		.info = "quit microcom",
	}, {
		.name = "help",
		.fn = cmd_help,
		.info = "show help",
	}, {
		.name = "x",
		.fn = cmd_execute,
		.info = "execute a script",
		.help = "x <scriptfile>",
	}, {
		.name = "#",
		.fn = cmd_comment,
		.info = "comment",
	},
};

void commands_init(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(cmds); i++)
		register_command(&cmds[i]);
}