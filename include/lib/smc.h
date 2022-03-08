

typedef struct {
	/* Function identifier. Identifies which function is being invoked. */
	uint32_t	fid;

	u_register_t	arg1;
	u_register_t	arg2;
	u_register_t	arg3;
	u_register_t	arg4;
	u_register_t	arg5;
	u_register_t	arg6;
	u_register_t	arg7;
} smc_args;

/* SMC calls can return up to 8 register values */
typedef struct {
	u_register_t	ret0;
	u_register_t	ret1;
	u_register_t	ret2;
	u_register_t	ret3;
	u_register_t	ret4;
	u_register_t	ret5;
	u_register_t	ret6;
	u_register_t	ret7;
} smc_ret_values;

smc_ret_values plat_smc(const smc_args *args);
