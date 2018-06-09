#include <assert.h>
#include <errno.h>
#include <inttypes.h>

#include <ply/ply.h>
#include <ply/internal.h>

#include "built-in.h"


static int pid_fprint(struct type *t, FILE *fp, const void *data)
{
	return fprintf(fp, "%5"PRIu32, *((uint32_t *)data)); 
}

struct type t_pid = {
	.ttype = T_TYPEDEF,

	.tdef = {
		.name = ":pid",
		.type = &t_u32,
		.fprint = pid_fprint,
	},
};

struct type t_pid_func = {
	.ttype = T_FUNC,

	.func = { .type = &t_pid },
};

static int pid_ir_post(const struct func *func, struct node *n,
				struct ply_probe *pb)
{
	struct node *ptr = n->expr.args;

	ir_init_sym(pb->ir, n->sym);

	ir_emit_insn(pb->ir, CALL(BPF_FUNC_get_current_pid_tgid), 0, 0);
	ir_emit_insn(pb->ir, ALU64_IMM(BPF_RSH, 32), BPF_REG_0, 0);
	ir_emit_reg_to_sym(pb->ir, n->sym, BPF_REG_0);
	return 0;
}

__ply_built_in const struct func pid_func = {
	.name = "pid",
	.type = &t_pid_func,
	.static_ret = 1,

	.ir_post = pid_ir_post,
};


static int kpid_ir_post(const struct func *func, struct node *n,
				struct ply_probe *pb)
{
	struct node *ptr = n->expr.args;

	ir_init_sym(pb->ir, n->sym);

	ir_emit_insn(pb->ir, CALL(BPF_FUNC_get_current_pid_tgid), 0, 0);
	ir_emit_insn(pb->ir, ALU64_IMM(BPF_AND, 0xffffffff), BPF_REG_0, 0);
	ir_emit_reg_to_sym(pb->ir, n->sym, BPF_REG_0);
	return 0;
}

__ply_built_in const struct func kpid_func = {
	.name = "kpid",
	.type = &t_pid_func,
	.static_ret = 1,

	.ir_post = kpid_ir_post,
};


/* uid/gid */

static int uid_fprint(struct type *t, FILE *fp, const void *data)
{
	return fprintf(fp, "%4"PRIu32, *((uint32_t *)data)); 
}

struct type t_uid = {
	.ttype = T_TYPEDEF,

	.tdef = {
		.name = ":uid",
		.type = &t_u32,
		.fprint = uid_fprint,
	},
};

struct type t_uid_func = {
	.ttype = T_FUNC,

	.func = { .type = &t_uid },
};

static int uid_ir_post(const struct func *func, struct node *n,
				struct ply_probe *pb)
{
	struct node *ptr = n->expr.args;

	ir_init_sym(pb->ir, n->sym);

	ir_emit_insn(pb->ir, CALL(BPF_FUNC_get_current_uid_gid), 0, 0);
	ir_emit_insn(pb->ir, ALU64_IMM(BPF_AND, 0xffffffff), BPF_REG_0, 0);
	ir_emit_reg_to_sym(pb->ir, n->sym, BPF_REG_0);
	return 0;
}


__ply_built_in const struct func uid_func = {
	.name = "uid",
	.type = &t_uid_func,
	.static_ret = 1,

	.ir_post = uid_ir_post,
};



static int gid_ir_post(const struct func *func, struct node *n,
				struct ply_probe *pb)
{
	struct node *ptr = n->expr.args;

	ir_init_sym(pb->ir, n->sym);

	ir_emit_insn(pb->ir, CALL(BPF_FUNC_get_current_uid_gid), 0, 0);
	ir_emit_insn(pb->ir, ALU64_IMM(BPF_RSH, 32), BPF_REG_0, 0);
	ir_emit_reg_to_sym(pb->ir, n->sym, BPF_REG_0);
	return 0;
}

__ply_built_in const struct func gid_func = {
	.name = "gid",
	.type = &t_uid_func,
	.static_ret = 1,

	.ir_post = gid_ir_post,
};


static int cpu_fprint(struct type *t, FILE *fp, const void *data)
{
	return fprintf(fp, "%2"PRIu32, *((uint32_t *)data)); 
}

struct type t_cpu = {
	.ttype = T_TYPEDEF,

	.tdef = {
		.name = ":cpu",
		.type = &t_u32,
		.fprint = cpu_fprint,
	},
};

struct type t_cpu_func = {
	.ttype = T_FUNC,

	.func = { .type = &t_cpu },
};

static int cpu_ir_post(const struct func *func, struct node *n,
				struct ply_probe *pb)
{
	struct node *ptr = n->expr.args;

	ir_init_sym(pb->ir, n->sym);

	ir_emit_insn(pb->ir, CALL(BPF_FUNC_get_smp_processor_id), 0, 0);
	ir_emit_reg_to_sym(pb->ir, n->sym, BPF_REG_0);
	return 0;
}

__ply_built_in const struct func cpu_func = {
	.name = "cpu",
	.type = &t_cpu_func,
	.static_ret = 1,

	.ir_post = cpu_ir_post,
};


struct type t_comm = {
	.ttype = T_ARRAY,

	.array = {
		.type = &t_char,
		.len = 16,
	},
};

struct type t_comm_func = {
	.ttype = T_FUNC,

	.func = { .type = &t_comm },
};

static int comm_ir_post(const struct func *func, struct node *n,
				struct ply_probe *pb)
{
	struct node *ptr = n->expr.args;

	ir_init_sym(pb->ir, n->sym);

	ir_emit_bzero(pb->ir, n->sym->irs.stack, type_sizeof(n->sym->type));

	ir_emit_insn(pb->ir, MOV, BPF_REG_1, BPF_REG_BP);
	ir_emit_insn(pb->ir, ALU_IMM(BPF_ADD, n->sym->irs.stack), BPF_REG_1, 0);
	ir_emit_insn(pb->ir, MOV_IMM(type_sizeof(n->sym->type)), BPF_REG_2, 0);
	ir_emit_insn(pb->ir, CALL(BPF_FUNC_get_current_comm), 0, 0);
	return 0;
}

__ply_built_in const struct func comm_func = {
	.name = "comm",
	.type = &t_comm_func,
	.static_ret = 1,

	.ir_post = comm_ir_post,
};

__ply_built_in const struct func execname_func = {
	/* alias to comm */
	.name = "execname",
	.type = &t_comm_func,
	.static_ret = 1,

	.ir_post = comm_ir_post,
};


struct type t_time = {
	.ttype = T_TYPEDEF,

	.tdef = {
		.name = ":time",
		.type = &t_s64,

		/* .fprint = time_fprint, */
	},
};

struct type t_time_func = {
	.ttype = T_FUNC,

	.func = { .type = &t_time },
};

static int time_ir_post(const struct func *func, struct node *n,
			       struct ply_probe *pb)
{
	struct node *ptr = n->expr.args;

	ir_init_sym(pb->ir, n->sym);

	ir_emit_insn(pb->ir, CALL(BPF_FUNC_ktime_get_ns), 0, 0);
	ir_emit_reg_to_sym(pb->ir, n->sym, BPF_REG_0);
	return 0;
}

__ply_built_in const struct func time_func = {
	.name = "time",
	.type = &t_time_func,
	.static_ret = 1,

	.ir_post = time_ir_post,
};
