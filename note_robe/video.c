//===================================epdc
vidinfo_t panel_info = {
	.vl_refresh = 85,
	.vl_col = 1024,
	.vl_row = 758,
	.vl_pixclock = 40000000,
	.vl_left_margin = 12,
	.vl_right_margin = 76,
	.vl_upper_margin = 4,
	.vl_lower_margin = 5,
	.vl_hsync = 12,
	.vl_vsync = 2,
	.vl_sync = 0,
	.vl_mode = 0,
	.vl_flag = 0,
	.vl_bpix = 3,
	.cmap = 0,
};

typedef struct vidinfo {
	u_long vl_refresh;      /* Refresh Rate Hz */
	u_long vl_row;          /* resolution in x */
	u_long vl_col;          /* resolution in y */
	u_long vl_rot;
	u_long vl_pixclock;     /* pixel clock in picoseconds */
	u_long vl_left_margin;  /* Horizontal back porch */
	u_long vl_right_margin; /* Horizontal front porch */
	u_long vl_upper_margin; /* Vertical back porch */
	u_long vl_lower_margin; /* Vertical front porch */
	u_long vl_hsync;        /* Horizontal sync pulse length */
	u_long vl_vsync;        /* Vertical sync pulse length */
	u_long vl_sync;         /* Polarity on data enable */
	u_long vl_mode;         /* Video Mode */
	u_long vl_flag;
	u_char  vl_bpix;
	ushort  *cmap;
	struct epdc_data_struct epdc_data;
} vidinfo_t;
struct epdc_data_struct {
	/* EPDC buffer pointers */
	u_long working_buf_addr;
	u_long waveform_buf_addr;

	/* Waveform mode definitions */
	struct waveform_modes wv_modes;
	struct epdc_timing_params epdc_timings;
};
struct waveform_modes {
	int mode_init;
	int mode_du;
	int mode_gc4;
	int mode_gc8;
	int mode_gc16;
	int mode_gc32;
};
struct epdc_timing_params {
    int vscan_holdoff;
    int sdoed_width;
    int sdoed_delay;
    int sdoez_width;
    int sdoez_delay;
    int gdclk_hp_offs;
    int gdsp_offs;
    int gdoe_offs;
    int gdclk_offs;
    int num_ce;
};
//===========================================lcd
struct display_info_t {
	int	bus;
	int	addr;
	int	pixfmt;
	int	(*detect)(struct display_info_t const *dev);
	void	(*enable)(struct display_info_t const *dev);
	struct	fb_videomode mode;
};
struct display_info_t const displays[] = {{
	.bus = MX6UL_LCDIF1_BASE_ADDR,				//(graphicDevice)pannel.isabase = bus
	.addr = 0,
	.pixfmt = 24,								//depth = pixfmt
	.detect = NULL,
	.enable	= do_enable_parallel_lcd,
	.mode	= {									//fbmode = mode
		.name			= "TFT43AB",
		.xres           = 480,
		.yres           = 272,
		.pixclock       = 108695,
		.left_margin    = 8,
		.right_margin   = 4,
		.upper_margin   = 2,
		.lower_margin   = 4,
		.hsync_len      = 41,
		.vsync_len      = 10,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} } };//board/myir/mys_imx6ull/mys_imx6ull.c:line 825
struct fb_videomode {
	const char *name;	/* optional */
	u32 refresh;		/* optional */
	u32 xres;
	u32 yres;
	u32 pixclock;
	u32 left_margin;
	u32 right_margin;
	u32 upper_margin;
	u32 lower_margin;
	u32 hsync_len;
	u32 vsync_len;
	u32 sync;
	u32 vmode;
	u32 flag;
};							//fbmode
typedef struct graphic_device {
    unsigned int isaBase;
    unsigned int pciBase;
    unsigned int dprBase;
    unsigned int vprBase;
    unsigned int cprBase;
    unsigned int frameAdrs;
    unsigned int memSize;
    unsigned int mode;
    unsigned int gdfIndex;
    unsigned int gdfBytesPP;
    unsigned int fg;
    unsigned int bg;
    unsigned int plnSizeX;
    unsigned int plnSizeY;
    unsigned int winSizeX;
    unsigned int winSizeY;
    char modeIdent[80];
} ;							//GraphicDevice




