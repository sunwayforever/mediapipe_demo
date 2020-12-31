#ifndef __DPEPARAMVALIDATION__
#define __DPEPARAMVALIDATION__

#include "CalibrationDataAll.h"
class DPEParamValidation;
namespace  InuCommon
{
class DPEDepthOpticalData : private InuDev::COpticalData
{  
	friend class DPEParamValidation;
        public:

    /// \brief    Constructor.
    /// \param[in] iOpticalData    Optical Data object.
    /// \param[in,out] iDownScaleFactor    
    /// \param[in,out] iHorizPadding    
    /// \param[in,out] iVerticalPadding   
    DPEDepthOpticalData(const InuDev::COpticalData& iOpticalData,
        float iDownScaleFactor = 1.0f,
        short iHorizPadding = 0,
        short iVerticalPadding = 0) : 
        InuDev::COpticalData(iOpticalData),
            mDownScaleFactor(iDownScaleFactor), mHorizPadding(iHorizPadding), mVerticalPadding(iVerticalPadding)
    {
		float f=16; //for A0=2, for B=16  -  subpixel
        mOpticalFactor = f*FocalLengthBaseLeft.X() * TranslationRectifiedLeft.X() / mDownScaleFactor;		// factor 2 is because disparity is on half pixel units
        mDisparityOffset = f*(OpticalCenterBaseRight.X() - OpticalCenterBaseLeft.X())/mDownScaleFactor - f*mHorizPadding;
		mZmin=0;
    }

    /// \brief  Down scale factor - for Binning set 2, for upscale X2 set 0.5
    float mDownScaleFactor;

    ///  \brief  Padding (added empty Pixels) on the left of the left image and on the
    ///  right of the right image. For crop use negative values
    short mHorizPadding;
    short mVerticalPadding; 

    ///  \brief   Used for Disparity to Depth calculation
    float mOpticalFactor;
    float mDisparityOffset;
	float mZmin;
};
};

class DPEParamValidation
{
private:
    InuDev::COpticalData mOpticalData;


	int m_sizex;
	int m_sizey;

	int m_scenario_mode;
	int m_scenario_range;

	int m_active_pattern;
	int m_noise_reduction;
	int m_disable_machines;
	int m_decimation;

	


	int m_inout_sizex;
	int m_b00_sizex;
	int m_b01_sizex;
	int m_b00_sizex_before;
	int m_b01_sizex_before;
	int m_b00_l;
	int m_b00_r;
	int m_b01_l;
	int m_b01_r;

	int m_bA_l;
	int m_bA_r;
	int m_bA_t;

	int m_bB_l;
	int m_bB_r;
	int m_bB_t;

	int m_num_blocks;
	int m_bypass_active;
	int m_hybrid_row_active;
	int m_ds_level;
	int m_ds_sz_delta;
	int m_us_level;
	int m_us_sz_delta;
	
	int m_hands_upsample;


	int m_max_disparity;
	int m_min_disparity;
	int m_output_min_disparity;
	int m_roi_min_disparity;
	int m_roi_max_disparity;

	int m_merge_active;
	//bool doDDIFilter
	//resize SAD

	static const int m_DS_FILTER_SIZE=15;
	static const int m_ALIGNMENT=4;
	static const int m_HANDS_DISPARITY=30;
	int m_US_LEFT_EDGE;
	int m_US_RIGHT_EDGE;
	int m_DS_FILTER_EDGE;

//;mode={0-basic(single res), 1-details, 2-background, 3-low-res(decimate),4-high-res (upsample),5-economy}
//;range={0-basic(144), 1-extended (240),  2-very extended (144*2), 3-extreme (140*3), 4-limited (70), 5-economy(120)}

	static const int m_MODE_BASIC=0;
	static const int m_MODE_DETAILS=1;
	static const int m_MODE_BACKGROUND=2;
	static const int m_MODE_LOWRES=3;
	static const int m_MODE_HIGHRES=4;
	static const int m_MODE_ECONOMY=5;
	static const int m_MODE_ROI=6;
	static const int m_MODE_DOUBLE=7;
	//static const int m_MODE_HALF=8;
	static const int m_MODE_BYPASS=99;

	static const int m_RANGE_FULL=0; //144
	static const int m_RANGE_EXTENDED=1; //240
	static const int m_RANGE_VERY_EXTENDED=2;//288
	static const int m_RANGE_EXTREME=3;//432
	static const int m_RANGE_LIMITED=4;//72
	static const int m_RANGE_ECONOMY=5;//120
	static const int m_RANGE_ROI=6;//120 above first 40
	static const int m_RANGE_96=7;
	//static const int m_RANGE_192=8;

	int m_decimation_size;
	int m_decimation_value;


public:

	void doExecuteFull(int sizex,int sizey)
	{
		bool is_default;
		bool is_present;

		is_present=getConfigRegister("mode", m_scenario_mode,is_default);
		if(m_scenario_mode==m_MODE_BYPASS)
		{
			return;
		}

		Sleep(20);
		doExecuteScenario();
		reloadRegisters();
		Sleep(20);
		doExecuteValidation(sizex,sizey);
		reloadRegisters();
		Sleep(20);
	}


	void doExecuteScenario()
	{
		bool is_default;
		bool is_present;

		is_present=getConfigRegister("mode", m_scenario_mode,is_default);
		
		if(!is_present)
		{
			m_scenario_mode=-1;
		}
		is_present=getConfigRegister("range", m_scenario_range,is_default);
		if(!is_present)
		{
			m_scenario_range=-1;
		}
		setDefaultScenario(); 
		setScenarioRange();
		setScenarioMode();
	}

	void doExecuteValidation(int sizex,int sizey)
	{
		m_sizex=sizex;
		m_sizey=sizey;
		if(m_sizey>YDIM_MAX)
		{
			printf("ERROR: IMAGE HEIGHT ABOVE 1080!!!\n");
		}
		if(m_sizey>XDIM_MAX)
		{
			printf("ERROR: IMAGE WIDTH ABOVE 1920!!!\n");
		}

		bool is_default;
		getConfigRegister("active_pattern", m_active_pattern,is_default);
		getConfigRegister("noise_reduction", m_noise_reduction,is_default);
		getConfigRegister("disable_machines", m_disable_machines,is_default);
		//configIAEBypass(); Do not call this! Cannot bypass blocks in param validation path!
		readDisparityRange();
		configMultiresolution(); 
		doConfig();
		doValidate();
		// new for update DPE output size registers
		GetDPEOutSizes();
		writeToDB();
		testOverride();
	}

	
private:
//====================================================================================
//	
//	READ	
//
//  Here I mainly get config reg presets and set HW reg sets
//====================================================================================
	
	virtual void reloadRegisters(){};
	virtual void setDpeRegister(const char* register_name, const char* field_name, int value){};
	virtual void setIaeRegister(const char* register_name, const char* field_name, int value){};
	virtual void setConfigRegister(const char* register_name, const char* field_name, int value){};
	virtual int getDpeRegister(const char* register_name, const char* field_name){return 0;};
	virtual bool getConfigRegister(const char* field_name, int &value, bool &is_default){return false;};
	void setDpeRegisterWithDecrement(const char* register_name, const char* field_name, int value)
	{
		setDpeRegister(register_name, field_name, value-1);
	}
	int getDpeRegisterWithIncrement(const char* register_name, const char* field_name)
	{
		return getDpeRegister(register_name, field_name)+1;
	}



	



	


//====================================================================================
//	
//	SCENARIO
//
//  Here I set the top level of parameters for scenario
//====================================================================================
	


	void setScenarioMode()
	{
		bool is_default; 
		getConfigRegister("maxdisparity", m_max_disparity, is_default);
		if(m_scenario_mode==m_MODE_BASIC)
		{
			//setConfigRegister("validation","unite_strength_b0",0);
			setConfigRegister("validation","active_pattern",0);
			//setConfigRegister("validation","mask_threshold",16);
			//setConfigRegister("validation","mask_threshold",32);
			setConfigRegister("validation","mask_threshold",24);
			setConfigRegister("validation","penalties_scheme_b0",242);
		}
		else if(m_scenario_mode==m_MODE_DETAILS)
		{
			if(m_max_disparity>143)
			{
				setConfigRegister("validation","merge_scheme",142);
			}
			else
			{
				setConfigRegister("validation","merge_scheme",141);
			}
		}
		else if((m_scenario_mode==m_MODE_BACKGROUND) || (m_scenario_mode==m_MODE_DOUBLE))
		{
			if(m_max_disparity>143)
			{
				setConfigRegister("validation","merge_scheme",242);
			}
			else
			{
				setConfigRegister("validation","merge_scheme",241);
			}
			setConfigRegister("validation","noise_reduction",2);
			setConfigRegister("validation","active_pattern",3);

		}
		else if(m_scenario_mode==m_MODE_HIGHRES)
		{
			//setConfigRegister("validation","merge_scheme",312);
			setConfigRegister("validation","merge_scheme",142);
			//setConfigRegister("validation","merge_scheme",141);
			setConfigRegister("validation","noise_reduction",2);
			setConfigRegister("validation","mask_threshold",24);   
			setConfigRegister("validation","sadsize",0);
			setConfigRegister("validation","active_pattern",0);
			//setConfigRegister("validation","ddifilter",4);
		}
		else if(m_scenario_mode==m_MODE_LOWRES)
		{
			if(m_max_disparity>143)
			{
				setConfigRegister("validation","merge_scheme",332);
			}
			else
			{
				setConfigRegister("validation","merge_scheme",331);
			}
			setConfigRegister("validation","noise_reduction",3);
			setConfigRegister("validation","ddifilter",4);
			setConfigRegister("validation","unite_strength_b0",8);
			setConfigRegister("validation","unite_strength_b01",8);
			setConfigRegister("validation","mask_threshold",24);  
			setConfigRegister("validation","active_pattern",3);
			setConfigRegister("validation","penalties_scheme_b0",243);
			//setConfigRegister("validation","penalties_scheme_b01",135);
		}
		else if(m_scenario_mode==m_MODE_ECONOMY)
		{
			setConfigRegister("validation","merge_scheme",221);
			setConfigRegister("validation","noise_reduction",1);
			setConfigRegister("validation","disable_machines",1); //DISABLE ROI MACHINES
		}
		else if(m_scenario_mode==m_MODE_ROI)
		{
			setConfigRegister("validation","merge_scheme",111);
			setConfigRegister("validation","noise_reduction",1);
			setConfigRegister("validation","disable_machines",2); //DISABLE DS MACHINES

		}

	}

	void setDefaultScenario()
	{
		setConfigRegister("validation","mindisparity",0);
		setConfigRegister("validation","maxdisparity",143);
		setConfigRegister("validation","sadsize",0);
		setConfigRegister("validation","ddifilter",1);
		setConfigRegister("validation","multiresolution",0);
		setConfigRegister("validation","penalties_b0",4);
		setConfigRegister("validation","penalties_b01",6);
		setConfigRegister("validation","penalties_scheme_b0",240);
		setConfigRegister("validation","penalties_scheme_b01",132);
		setConfigRegister("validation","merge_scheme",142);
		setConfigRegister("validation","unite_strength_b0",4);
		setConfigRegister("validation","unite_strength_b01",0);
		setConfigRegister("validation","mask_threshold",16);
		setConfigRegister("validation","active_pattern",2);
		setConfigRegister("validation","noise_reduction",0);
	}

	void setScenarioRange()
	{
		if(m_scenario_mode==m_MODE_HIGHRES)
		{
			//setConfigRegister("validation","multiresolution",22);
			setConfigRegister("validation","multiresolution",21);
		}
		if(m_scenario_mode==m_MODE_DOUBLE)
		{
			setConfigRegister("validation","multiresolution",11);
		}
		if(m_scenario_mode==m_MODE_LOWRES)
		{
			setConfigRegister("validation","multiresolution",13);
		}
		if(m_scenario_mode==m_MODE_ECONOMY)
		{
			setConfigRegister("validation","multiresolution",12);
		}
		if(m_scenario_mode==m_MODE_ROI)
		{
			setConfigRegister("validation","multiresolution",12);
		}
		if(m_scenario_range==m_RANGE_FULL)
		{
			setConfigRegister("validation","maxdisparity",144-1);
			if(m_scenario_mode==-1)
			{
				m_scenario_mode=m_MODE_BASIC;
			}
		}
		if(m_scenario_range==m_RANGE_96)
		{
			setConfigRegister("validation","maxdisparity",96-1);
			if(m_scenario_mode==-1)
			{
				m_scenario_mode=m_MODE_DOUBLE;
			}
		}
		else if(m_scenario_range==m_RANGE_ECONOMY)
		{
			setConfigRegister("validation","maxdisparity",120-1);
			if(m_scenario_mode==-1)
			{
				m_scenario_mode=m_MODE_BASIC;
			}
		}
		else if(m_scenario_range==m_RANGE_ROI)
		{
			setConfigRegister("validation","maxdisparity",120-1+40);
			setConfigRegister("validation","mindisparity",40);
			if(m_scenario_mode==-1)
			{
				m_scenario_mode=m_MODE_BASIC;
			}
		}
		else if(m_scenario_range==m_RANGE_EXTENDED)
		{
			setConfigRegister("validation","maxdisparity",240-1);
			if((m_scenario_mode==m_MODE_BASIC) || (m_scenario_mode==m_MODE_HIGHRES))
			{
				printf("WARNING BAD COMBINATION OF RANGE %d AND MODE %d!!!\n\n",m_scenario_range,m_scenario_mode);
			}
			if(m_scenario_mode==-1)
			{
				m_scenario_mode=m_MODE_DETAILS;
			}
			if((m_scenario_mode==m_MODE_DETAILS) || (m_scenario_mode==m_MODE_BACKGROUND))
			{
				setConfigRegister("validation","multiresolution",12);
			}
		}
		else if(m_scenario_range==m_RANGE_VERY_EXTENDED)
		{
			setConfigRegister("validation","maxdisparity",144*2-1);
			if((m_scenario_mode==m_MODE_BASIC) || (m_scenario_mode==m_MODE_HIGHRES))
			{
				printf("WARNING BAD COMBINATION OF RANGE %d AND MODE %d!!!\n\n",m_scenario_range,m_scenario_mode);
			}
			if(m_scenario_mode==-1)
			{
				m_scenario_mode=m_MODE_BACKGROUND;
			}
			if((m_scenario_mode==m_MODE_DETAILS) || (m_scenario_mode==m_MODE_BACKGROUND))
			{
				setConfigRegister("validation","multiresolution",12);
			}
		}
		else if(m_scenario_range==m_RANGE_EXTREME)
		{
			setConfigRegister("validation","maxdisparity",144*3-1);
			if((m_scenario_mode==m_MODE_BASIC) || (m_scenario_mode==m_MODE_HIGHRES))
			{
				printf("WARNING BAD COMBINATION OF RANGE %d AND MODE %d!!!\n\n",m_scenario_range,m_scenario_mode);
			}
			if(m_scenario_mode==-1)
			{
				m_scenario_mode=m_MODE_BACKGROUND;
			}
			if((m_scenario_mode==m_MODE_DETAILS) || (m_scenario_mode==m_MODE_BACKGROUND))
			{
				setConfigRegister("validation","multiresolution",13);
			}
		}
		else if(m_scenario_range==m_RANGE_LIMITED)
		{
			setConfigRegister("validation","maxdisparity",144/2-1);
			if((m_scenario_mode==m_MODE_DETAILS) || (m_scenario_mode==m_MODE_BACKGROUND) || (m_scenario_mode==m_MODE_LOWRES))
			{
				printf("WARNING BAD COMBINATION OF RANGE %d AND MODE %d!!!\n\n",m_scenario_range,m_scenario_mode);
			}
			if(m_scenario_mode==-1)
			{
				m_scenario_mode=m_MODE_BASIC;
			}
			if(m_scenario_mode==m_MODE_HIGHRES)
			{
				setConfigRegister("validation","multiresolution",21);
			}
		}
		if(m_scenario_range==-1)
		{
			if((m_scenario_mode==m_MODE_DETAILS) || (m_scenario_mode==m_MODE_BACKGROUND) || (m_scenario_mode==m_MODE_LOWRES))
			{
				m_scenario_range=m_RANGE_VERY_EXTENDED;
			}
			else if( (m_scenario_mode==m_MODE_HIGHRES))
			{
				m_scenario_range=m_RANGE_LIMITED;
			}
			else
			{
				m_scenario_range=m_RANGE_FULL;
			}
			setScenarioRange();
		}
	}

//====================================================================================
//	
//	CONFIG	
//
//  Here I mainly get config reg presets and set HW reg sets
//====================================================================================

	void doConfig()
	{
		configCorrectDefaults(); 

		configDDIFilter(); 
		configMergeScheme();
		configSadSize();
		configPenalties();
		configUnite();
		configOptimizationMask();
		configStatistics();
		configDecimation();
	}

	void configCorrectDefaults()
	{
		setDpeRegister("dpe_alg_f_opt_sub_pix","accuracy",3);
		setDpeRegister("dpe_alg_f_opt_sub_pix","th1",6);
		setDpeRegister("dpe_alg_f_opt_sub_pix","th2",24);
		setDpeRegister("dpe_alg_f_opt_sub_pix","th3",48);
		setDpeRegister("dpe_alg_f_opt_sub_pix","th4",96);

		
		
		//#define PERFORM_3_WAY_OPTIMIZATION
		//MAX VALUE IS 3!!!
		#ifndef PERFORM_3_WAY_OPTIMIZATION
			setDpeRegister("dpe_alg_f0_opt_dir_factor","l2r",2);
			setDpeRegister("dpe_alg_f0_opt_dir_factor","u2d",2);
			setDpeRegister("dpe_alg_f01_opt_dir_factor","l2r",2);
			setDpeRegister("dpe_alg_f01_opt_dir_factor","u2d",2);
		#else
			setDpeRegister("dpe_alg_f0_opt_dir_factor","l2r",3);
			setDpeRegister("dpe_alg_f0_opt_dir_factor","u2d",2);
			setDpeRegister("dpe_alg_f01_opt_dir_factor","l2r",3);
			setDpeRegister("dpe_alg_f01_opt_dir_factor","u2d",2);
		#endif

		setDpeRegisterWithDecrement("dpe_alg_s_disp_limit","max",240);


		setDpeRegister("dpe_alg_f0_conf","sp_punish",1);
		setDpeRegister("dpe_alg_f01_conf","sp_punish",1);

		setDpeRegister("dpe_alg_f_conf_fz", "mult_a_0", 7); //UNITE - good, mostly values 15 (takes right input)
		setDpeRegister("dpe_alg_f_conf_fz", "mult_b_0", 7); //L2R - good, with small issues on subpixel

		setDpeRegister("dpe_alg_f_cost_measure", "th1", 6);
		setDpeRegister("dpe_alg_f_cost_measure", "th2", 24);
		setDpeRegister("dpe_alg_f_cost_measure", "th3", 256);

		setDpeRegister("dpe_alg_r0_side_match", "num_disp", 1); 
		setDpeRegister("dpe_alg_r0_side_match", "num_disp_ref", 1); 
		setDpeRegister("dpe_alg_r0_side_match", "rng_width", 1); 
		setDpeRegister("dpe_alg_r0_side_match", "num_compared", 1); 

		setDpeRegister("dpe_alg_r01_side_match", "num_disp", 1); 
		setDpeRegister("dpe_alg_r01_side_match", "num_disp_ref", 1); 
		setDpeRegister("dpe_alg_r01_side_match", "rng_width", 1); 
		setDpeRegister("dpe_alg_r01_side_match", "num_compared", 1); 


		setDpeRegister("dpe_alg_f_smt_prep", "shift", 7);
		setDpeRegister("dpe_alg_f_smt_prep", "offset", 2);

		setDpeRegister("dpe_alg_f_smt", "th1", 8);
		setDpeRegister("dpe_alg_f_smt", "th2", 96);
		setDpeRegister("dpe_alg_f_smt", "th3", 255);

		setDpeRegister("dpe_alg_f_conf_fz", "mult_a_1", 7); //SMT 
		//Needs at least 1 disp, disp_ref, width_comp, otherwise too high
		setDpeRegister("dpe_alg_f_conf_fz", "mult_b_1", 7); //COST - SEMI
		setDpeRegister("dpe_alg_f_conf_fz", "mult_a_2", 12); //FZ 0
		setDpeRegister("dpe_alg_f_conf_fz", "mult_b_2", 7); //FZ 1


	}

	void configOptimizationMask()
	{
		if(m_active_pattern==9)
		{
			setDpeRegister("dpe_alg_cf01_msk_cond1_prp","y_th",2048);
			setDpeRegister("dpe_alg_cf01_msk_cond1_prp","u_th",16);
			setDpeRegister("dpe_alg_cf01_msk_cond1_prp","v_th",16);

			setDpeRegister("dpe_alg_cf01_msk_cnd12_ctr","y_th",2048);
			setDpeRegister("dpe_alg_cf01_msk_cnd12_ctr","u_th",16);
			setDpeRegister("dpe_alg_cf01_msk_cnd12_ctr","v_th",16);

			setDpeRegister("dpe_alg_cf0_msk_cond1_prp","y_th",2048);
			setDpeRegister("dpe_alg_cf0_msk_cond1_prp","u_th",16);
			setDpeRegister("dpe_alg_cf0_msk_cond1_prp","v_th",16);

			setDpeRegister("dpe_alg_cf0_msk_cnd12_ctr","y_th",2048);
			setDpeRegister("dpe_alg_cf0_msk_cnd12_ctr","u_th",16);
			setDpeRegister("dpe_alg_cf0_msk_cnd12_ctr","v_th",16);

			setDpeRegister("dpe_alg_cf01_msk_opt","y_th",2048);
			setDpeRegister("dpe_alg_cf01_msk_opt","u_th",16);
			setDpeRegister("dpe_alg_cf01_msk_opt","v_th",16);

			setDpeRegister("dpe_alg_cf0_msk_opt","y_th",2048);
			setDpeRegister("dpe_alg_cf0_msk_opt","u_th",16);
			setDpeRegister("dpe_alg_cf0_msk_opt","v_th",16);
			return;
		}



		bool is_default;
		int f_center=1;
		int f_peri=1;
		int f_opti=1;
		int pv;
		getConfigRegister("mask_threshold", pv,is_default);
		
		if(m_active_pattern==1)
		{
			f_center=4;
			f_peri=4;
			f_opti=8;
		}
		if(m_active_pattern==2)
		{
			f_center=4;
			f_peri=1;
			f_opti=1;
		}
		if(m_active_pattern==4)
		{
			f_center=3;
			f_peri=2;
			f_opti=2;
		}
		if(m_active_pattern==3)
		{
			f_center=1;
			f_peri=4;
			f_opti=4;
		}
		int f_chroma=2;
		//U and V unproportional to y probably due to lack of RGB filter;
		int th_u=pv/f_chroma; //max=255;
		int th_v=th_u;
		int th_y=th_u*16;

		int ddifilter;
		getConfigRegister("ddifilter", ddifilter,is_default);
		int f_ds=1;
		if(ddifilter>1)
		{
			f_ds=2;
		}



		setDpeRegister("dpe_alg_cf01_msk_cond1_prp","y_th",th_y/f_ds*f_peri);
		setDpeRegister("dpe_alg_cf01_msk_cond1_prp","u_th",th_u/f_ds*f_peri);
		setDpeRegister("dpe_alg_cf01_msk_cond1_prp","v_th",th_v/f_ds*f_peri);

		setDpeRegister("dpe_alg_cf01_msk_cnd12_ctr","y_th",th_y/f_ds*f_center);
		setDpeRegister("dpe_alg_cf01_msk_cnd12_ctr","u_th",th_u/f_ds*f_center);
		setDpeRegister("dpe_alg_cf01_msk_cnd12_ctr","v_th",th_v/f_ds*f_center);

		setDpeRegister("dpe_alg_cf0_msk_cond1_prp","y_th",th_y*f_peri);
		setDpeRegister("dpe_alg_cf0_msk_cond1_prp","u_th",th_u*f_peri);
		setDpeRegister("dpe_alg_cf0_msk_cond1_prp","v_th",th_v*f_peri);

		setDpeRegister("dpe_alg_cf0_msk_cnd12_ctr","y_th",th_y*f_center);
		setDpeRegister("dpe_alg_cf0_msk_cnd12_ctr","u_th",th_u*f_center);
		setDpeRegister("dpe_alg_cf0_msk_cnd12_ctr","v_th",th_v*f_center);

		setDpeRegister("dpe_alg_cf01_msk_opt","y_th",th_y/f_ds*f_opti/2);
		setDpeRegister("dpe_alg_cf01_msk_opt","u_th",th_u/f_ds*f_opti/2);
		setDpeRegister("dpe_alg_cf01_msk_opt","v_th",th_v/f_ds*f_opti/2);

		setDpeRegister("dpe_alg_cf0_msk_opt","y_th",th_y*f_opti/2);
		setDpeRegister("dpe_alg_cf0_msk_opt","u_th",th_u*f_opti/2);
		setDpeRegister("dpe_alg_cf0_msk_opt","v_th",th_v*f_opti/2);

	}

	


	void configUnite()
	{
		configUniteBlock("0",m_us_level);
		configUniteBlock("01",m_ds_level);
	}


	void configUniteBlock(const char* block_num, int resample)
	{
		char tmp[256];
		bool is_default;
		int pv;
		sprintf(tmp,"unite_strength_b%s",block_num);
		getConfigRegister(tmp, pv,is_default);
		int v1, v2, v3, v4;
		v2=pv/2;
		v1=pv-v2;
		v3=pv/3;
		if(v1<1) v1=1;
		if(v2<1) v2=1;
		if(v3<1) v3=1;
		//v4=4;v4=2
		//v4=min(pv,3); //3;
		v4=4;
		sprintf(tmp,"dpe_alg_r%s_side_match",block_num);
		int min_ref=0;
		if(m_hybrid_row_active)
		{
			min_ref=1; //NEED CONFIDENCE!
		}

		setDpeRegisterWithDecrement(tmp,"num_disp",max(min_ref,v1));
		setDpeRegisterWithDecrement(tmp,"num_disp_ref",max(min_ref,v2));
		setDpeRegisterWithDecrement(tmp,"num_compared",max(min_ref,v3));
		setDpeRegisterWithDecrement(tmp,"rng_width",min(4,max(min_ref,v4*resample)));

	}

	void configIAEBypass()
	{
		bool is_default;
		int pv;
		getConfigRegister("iae_bypass", pv,is_default);
		if(pv==0)
		{
			return;
		}


		int i;
		char fieldName[256];

		//IAE ENABLE
		for(i=0;i<=13;i++)
		{
			sprintf(fieldName,"iam_ch%d_en",i);
			setIaeRegister("iae_enable",fieldName,1);
		}
		for(i=0;i<=5;i++)
		{
			sprintf(fieldName,"iam_ds_%d_en_cvl",i);
			setIaeRegister("iae_enable",fieldName,1);
			sprintf(fieldName,"iam_ds_%d_en_cvr",i);
			setIaeRegister("iae_enable",fieldName,1);
		}
		setIaeRegister("iae_enable","iau_cvl_en",1);
		setIaeRegister("iae_enable","iau_cvr_en",1);
		setIaeRegister("iae_enable","slu_cvl_en",1);
		setIaeRegister("iae_enable","slu_cvr_en",1);
		setIaeRegister("iae_enable","slu_cs_en",1);
		setIaeRegister("iae_enable","pwm_en",1);


		//IAE BYPASS 0

		setIaeRegister("iae_bypass_0","iau_cvl_bpc_bypass",2);
		setIaeRegister("iae_bypass_0","iau_cvl_dsr_bypass",2);
		setIaeRegister("iae_bypass_0","iau_cvl_dms_bypass",2);
		setIaeRegister("iae_bypass_0","iau_cvl_fir_bypass",2);
		setIaeRegister("iae_bypass_0","iau_cvl_gcr_bypass",2);
		setIaeRegister("iae_bypass_0","iau_cvl_ib_bypass",2);
		setIaeRegister("iae_bypass_0","iau_cvl_wb_bypass",2);

		setIaeRegister("iae_bypass_0","iau_cvr_bpc_bypass",2);
		setIaeRegister("iae_bypass_0","iau_cvr_dsr_bypass",2);
		setIaeRegister("iae_bypass_0","iau_cvr_dms_bypass",2);
		setIaeRegister("iae_bypass_0","iau_cvr_fir_bypass",2);
		setDpeRegister("iae_bypass_0","iau_cvr_gcr_bypass",2);
		setDpeRegister("iae_bypass_0","iau_cvr_ib_bypass",2);
		setIaeRegister("iae_bypass_0","iau_cvr_wb_bypass",2);

		//IAE BYPASS 1
		for(i=0;i<=6;i++)
		{
			sprintf(fieldName,"ch%d_%d_decimation",i,i+7);
			setIaeRegister("iae_bypass_1",fieldName,1);
		}
		for(i=0;i<=5;i++)
		{
			sprintf(fieldName,"ds_%d_bypass_cvl",i);
			setIaeRegister("iae_bypass_1",fieldName,2);
			sprintf(fieldName,"ds_%d_bypass_cvr",i);
			setIaeRegister("iae_bypass_1",fieldName,2);
		}

		setIaeRegister("iae_bypass_2","iau_cvl_median_bypass",1);

		setIaeRegister("iae_slu_cvl_control","pixel_division",7);
		setIaeRegister("iae_slu_cvl_control","pixel_width",3);
		setIaeRegister("iae_slu_cvr_control","pixel_division",7);
		setIaeRegister("iae_slu_cvr_control","pixel_width",3);

		setIaeRegister("iae_cvr_iau_yuv_ary_agy","a_ry",16384);
		setIaeRegister("iae_cvr_iau_yuv_ary_agy","a_gy",0);
		setIaeRegister("iae_cvr_iau_yuv_aby_by","a_by",0);
		setIaeRegister("iae_cvr_iau_yuv_aby_by","b_y",0);

		setIaeRegister("iae_cvr_iau_yuv_aru_agu","a_ru",0);
		setIaeRegister("iae_cvr_iau_yuv_aru_agu","a_gu",0);
		setIaeRegister("iae_cvr_iau_yuv_abu_bu","a_bu",0);
		setIaeRegister("iae_cvr_iau_yuv_abu_bu","b_u",0);

		setIaeRegister("iae_cvr_iau_yuv_arv_agv","a_rv",0);
		setIaeRegister("iae_cvr_iau_yuv_arv_agv","a_gv",0);
		setIaeRegister("iae_cvr_iau_yuv_abv_bv","a_bv",0);
		setIaeRegister("iae_cvr_iau_yuv_abv_bv","b_v",0);

		setIaeRegister("iae_cvl_iau_yuv_ary_agy","a_ry",16384);
		setIaeRegister("iae_cvl_iau_yuv_ary_agy","a_gy",0);
		setIaeRegister("iae_cvl_iau_yuv_aby_by","a_by",0);
		setIaeRegister("iae_cvl_iau_yuv_aby_by","b_y",0);

		setIaeRegister("iae_cvl_iau_yuv_aru_agu","a_ru",0);
		setIaeRegister("iae_cvl_iau_yuv_aru_agu","a_gu",0);
		setIaeRegister("iae_cvl_iau_yuv_abu_bu","a_bu",0);
		setIaeRegister("iae_cvl_iau_yuv_abu_bu","b_u",0);

		setIaeRegister("iae_cvl_iau_yuv_arv_agv","a_rv",0);
		setIaeRegister("iae_cvl_iau_yuv_arv_agv","a_gv",0);
		setIaeRegister("iae_cvl_iau_yuv_abv_bv","a_bv",0);
		setIaeRegister("iae_cvl_iau_yuv_abv_bv","b_v",0);
	}

	void configPenalties()
	{
		
		configPenaltiesBlock("penalties_b0","penalties_scheme_b0","dpe_alg_f0_opt_punish","dpe_alg_f0_opt_punish_idx");
		if(m_hybrid_row_active>0)
		{
			configPenaltiesBlock("penalties_b01","penalties_scheme_b01","dpe_alg_f01_opt_punish","dpe_alg_f01_opt_punish_idx");
		}
	}

	void configPenaltiesBlock(const char* reg_in, const char* reg_scheme_in, const char* reg_out,const char* reg_out_idx)
	{
		bool is_default;
		int pv;
		getConfigRegister(reg_in, pv,is_default);
		int psch;
		getConfigRegister(reg_scheme_in, psch,is_default);
		int p3sch=psch%10;
		int p2schT=(psch-p3sch)/10;
		int p2sch=p2schT%10;
		int p1sch=(p2schT-p2sch)/10;
		
		int p1,p2,p3;
		int p1s,p2s,p3s;
		int p3t;
		p1=p1sch*pv;
		p2=p2sch*pv;
		p3=p3sch*pv;
		if(p3>48)
		{
			p3=48;
		}
		if(p2+p3>63)
		{
			p2=63-p3;
		}
		if(p1+p3>63)
		{
			p1=63-p3;
		}
		p1s=p1/4; 
		p2s=p2/4;
		p3s=0;//p3/4;
		if(pv<8)
		{
			p3t=2;
		}
		else if(pv>16)
		{
			p3t=0;
		}
		else
		{
			p3t=1;
		}
		//p3t=1;
		setDpeRegister(reg_out,"p1",p1);
		setDpeRegister(reg_out,"p1_small",p1s);
		setDpeRegister(reg_out,"p2",p2);
		setDpeRegister(reg_out,"p2_small",p2s);
		setDpeRegister(reg_out_idx,"p3",p3);
		setDpeRegister(reg_out_idx,"p3_small",p3s);
		setDpeRegister(reg_out_idx,"tol",p3t);
	}

	void configMergeScheme()
	{
		bool is_default;
		int merge_scheme;
		getConfigRegister("merge_scheme", merge_scheme,is_default);
		int flag0=merge_scheme%10;
		int merge_schemeT=(merge_scheme-flag0)/10;
		int flag1=merge_schemeT%10;
		int flag2=(merge_schemeT-flag1)/10;
		
		if((flag0<1) || (flag0>2))
		{
			flag0=2;
			printf("WARNING: MergeUseRangeFlag OUT OF RANGE: %d\n",flag0);
		}
		if((flag1<1) || (flag1>4))
		{
			flag1=1;
			printf("WARNING: MergeOneGoodFlag OUT OF RANGE: %d\n",flag1);
		}
		if((flag2<1) || (flag2>4))
		{
			flag2=4;
			printf("WARNING: MergeTwoGoodFlag OUT OF RANGE: %d\n",flag2);
		}
		//setDpeRegisterWithDecrement("L2Rconsistency","MergeTwoGoodFlag",flag2);
		//setDpeRegisterWithDecrement("L2Rconsistency","MergeOneGoodFlag",flag1);
		//setDpeRegisterWithDecrement("L2Rconsistency","MergeUseRangeFlag",flag0);

		setDpeRegisterWithDecrement("dpe_alg_f_merge_ctrl","tgf",flag2);
		setDpeRegisterWithDecrement("dpe_alg_f_merge_ctrl","ogf",flag1);
		setDpeRegisterWithDecrement("dpe_alg_f_merge_ctrl","urf",flag0);

	}

	void configSadSize()
	{
		bool is_default;
		int sadsize;
		getConfigRegister("sadsize", sadsize,is_default);
		

		int periphery_height=11;
		int periphery_width_roi=15;
		int periphery_width_ds=15;

		// 1=Large supported SAD {11:11}
		// 2=Small reasonable SAD {7:7}
		// 3=Proportional large, {15:11} at ROI and {9:11} at DS
		// 4=Proportional small, {9:7} at ROI and {5:7} at DS
		if(sadsize==1)
		{
			periphery_height=11;
			periphery_width_roi=11;
			periphery_width_ds=11;
		}
		if(sadsize==2)
		{
			periphery_height=7;
			periphery_width_roi=7;
			periphery_width_ds=7;
		}
		if(sadsize==3)
		{
			periphery_height=11;
			periphery_width_roi=15;
			periphery_width_ds=9;
		}
		if(sadsize==4)
		{
			periphery_height=7;
			periphery_width_ds=5;
			periphery_width_roi=9;
		}
		if(sadsize==5)
		{
			//periphery_height=9;
			//periphery_width_ds=3;
			periphery_height=11;
			periphery_width_ds=7;
			periphery_width_roi=5;
		}
		setDpeRegister("dpe_alg_cf_msk_ver","diam",periphery_height);
		if(m_hybrid_row_active>0)
		{
			setDpeRegister("dpe_alg_cf0_msk_hor","diam",periphery_width_roi);
			setDpeRegister("dpe_alg_cf01_msk_hor","diam",periphery_width_ds);
			setDpeRegister("dpe_alg_f0_conf","agg_cnt_th",periphery_width_roi*periphery_height*4/5);
			setDpeRegister("dpe_alg_f01_conf","agg_cnt_th",periphery_width_ds*periphery_height*4/5);
			//setDpeRegister("dpe_alg_f0_conf","agg_punish",-1+16);
			//setDpeRegister("dpe_alg_f01_conf","agg_punish",-2+16);
		}
		else
		{
			setDpeRegister("dpe_alg_cf0_msk_hor","diam",periphery_width_ds);
			setDpeRegister("dpe_alg_f0_conf","agg_cnt_th",periphery_width_ds*periphery_height*4/5);
			//setDpeRegister("dpe_alg_f0_conf","agg_punish",-1+16);
		}
	
	}
	void configDDIFilter()
	{
		bool is_default;
		int ddifilter;
		getConfigRegister("ddifilter", ddifilter,is_default);
		if(is_default)
		{
			return;
		}
		//return; //NOT YET IMPLEMENTED
		
		int c[m_DS_FILTER_SIZE+2];
		configDDIFilterCoeffs(ddifilter,c);
		setSignDDIFilterCoeffs(ddifilter,c);

	
		setDpeRegister("dpe_ddi_alg_b01_ds_fir_gen","shift_val",c[m_DS_FILTER_SIZE]); //The default filter has shift 4. 0...8
		setDpeRegister("dpe_ddi_alg_b01_ds_fir_gen","offset_en",c[m_DS_FILTER_SIZE+1]); //The default offset is 0. 0/1

		int vclip0=8;
		int vclip=vclip0;
		if(m_noise_reduction>0)
		{
			vclip=64;
		}
		if(m_noise_reduction==1)
		{
			vclip0=vclip/2;
		}
		if(m_noise_reduction==2)
		{
			vclip0=0;
		}
		if(m_noise_reduction==3)
		{
			vclip0=vclip;
			vclip=0;
		}

#if 1
		setDpeRegister("dpe_alg_f0_gen","abs_diff_clip_low",vclip0); 
		setDpeRegister("dpe_alg_f01_gen","abs_diff_clip_low",vclip); 
#else
		setDpeRegister("dpe_alg_f0_gen","abs_diff_clip_low",0); 
		setDpeRegister("dpe_alg_f01_gen","abs_diff_clip_low",0); 

#endif
		if(m_noise_reduction>0)
		{
			setDpeRegister("dpe_alg_f_merge","punish_dec_ds",2);
			setDpeRegister("dpe_alg_f_merge","punish_dec_us",3);
		}
		else
		{
			setDpeRegister("dpe_alg_f_merge","punish_dec_ds",7);
			setDpeRegister("dpe_alg_f_merge","punish_dec_us",4);
		}


		setDpeRegister("dpe_ddi_alg_b01_ds_fir_0","c0",c[0]);
		setDpeRegister("dpe_ddi_alg_b01_ds_fir_0","c1",c[1]);
		setDpeRegister("dpe_ddi_alg_b01_ds_fir_0","c2",c[2]);
		setDpeRegister("dpe_ddi_alg_b01_ds_fir_0","c3",c[3]);
		setDpeRegister("dpe_ddi_alg_b01_ds_fir_0","c4",c[4]);

		setDpeRegister("dpe_ddi_alg_b01_ds_fir_1","c5",c[5]);
		setDpeRegister("dpe_ddi_alg_b01_ds_fir_1","c6",c[6]);
		setDpeRegister("dpe_ddi_alg_b01_ds_fir_1","c7",c[7]);
		setDpeRegister("dpe_ddi_alg_b01_ds_fir_1","c8",c[8]);
		setDpeRegister("dpe_ddi_alg_b01_ds_fir_1","c9",c[9]);

		setDpeRegister("dpe_ddi_alg_b01_ds_fir_2","c10",c[10]);
		setDpeRegister("dpe_ddi_alg_b01_ds_fir_2","c11",c[11]);
		setDpeRegister("dpe_ddi_alg_b01_ds_fir_2","c12",c[12]);
		setDpeRegister("dpe_ddi_alg_b01_ds_fir_2","c13",c[13]);
		setDpeRegister("dpe_ddi_alg_b01_ds_fir_2","c14",c[14]);


	}

	void setSignDDIFilterCoeffs(int ddifilter,int c[])
	{
		for (int i=0; i<m_DS_FILTER_SIZE; i++)
		{
			if(c[i]<0)
			{
				c[i]=(1<<5)+abs(c[i]);
			}
		}

	}


	
	void configDDIFilterCoeffs(int ddifilter,int c[])
	{
		int i;
		for (i=0; i<m_DS_FILTER_SIZE; i++)
		{
			c[i]=0;
		}
		int scale = 0;
		if(ddifilter==0) //Antialiasing
		{
			
			c[(m_DS_FILTER_SIZE-1)/2]=16;
			c[m_DS_FILTER_SIZE+1]=0;//do offset
			c[m_DS_FILTER_SIZE]=4;//shift
			return;
		}
		if(ddifilter==1) //Antialiasing
		{
			scale=configDDIFilterCoeffsAA(ddifilter,c);
			c[m_DS_FILTER_SIZE+1]=0;//do offset
			c[m_DS_FILTER_SIZE]=4;//shift
			return;
		}
		
		if(ddifilter==2) //HPF no DC
		{
			scale= configDDIFilterCoeffsHPF(ddifilter,c);
			c[m_DS_FILTER_SIZE+1]=1;//do offset
		}
		if(ddifilter==3) //HPF no DC
		{
			scale=configDDIFilterCoeffsDelta(ddifilter,c);
			
			c[m_DS_FILTER_SIZE+1]=1;//do offset
		}
		if(ddifilter==4) //Sum 2+3
		{
			int c1[m_DS_FILTER_SIZE];
			int scale1;
			int c2[m_DS_FILTER_SIZE];
			int scale2;
			int c3[m_DS_FILTER_SIZE];
			int scale3;
			for (i=0; i<m_DS_FILTER_SIZE; i++)
			{
				c1[i]=0;
				c2[i]=0;
				c3[i]=0;
			}
			scale1=configDDIFilterCoeffsAA(1,c1);
			scale2=configDDIFilterCoeffsHPF(2,c2);
			scale3=configDDIFilterCoeffsDelta(3,c3);
			scale=1;
			for (i=0; i<m_DS_FILTER_SIZE; i++)
			{
				c[i]=(scale1*c1[i]+scale2*c2[i]+scale3*c3[i])/6;
			}
			c[m_DS_FILTER_SIZE+1]=1;
		}
		for(int i=0;i<m_DS_FILTER_SIZE;i++)
		{
			c[i]=c[i]*scale;
		}
		c[m_DS_FILTER_SIZE]=5;//shift
	}


	int configDDIFilterCoeffsDelta(int ddifilter,int c[])
	{
		int scale=1;

			if(m_ds_level==1)
			{
				c[(m_DS_FILTER_SIZE-1)/2-1]=-1;
				c[(m_DS_FILTER_SIZE-1)/2]=2;
				c[(m_DS_FILTER_SIZE-1)/2+1]=-1;
				scale=16; //2*16=32
			}
			else if((m_ds_level==2) || (m_ds_level==5))
			{
				c[(m_DS_FILTER_SIZE-1)/2-3]=-1;
				c[(m_DS_FILTER_SIZE-1)/2-2]=-1;
				c[(m_DS_FILTER_SIZE-1)/2-1]=1;
				c[(m_DS_FILTER_SIZE-1)/2]=2; 
				c[(m_DS_FILTER_SIZE-1)/2+1]=1;
				c[(m_DS_FILTER_SIZE-1)/2+2]=-1;
				c[(m_DS_FILTER_SIZE-1)/2+3]=-1;
				scale=8; //8*4=32
			}
			else if(m_ds_level==3)
			{
				c[(m_DS_FILTER_SIZE-1)/2-4]=-1;
				c[(m_DS_FILTER_SIZE-1)/2-3]=-1;
				c[(m_DS_FILTER_SIZE-1)/2-2]=-1;
				c[(m_DS_FILTER_SIZE-1)/2-1]=2;
				c[(m_DS_FILTER_SIZE-1)/2]=2; 
				c[(m_DS_FILTER_SIZE-1)/2+1]=2;
				c[(m_DS_FILTER_SIZE-1)/2+2]=-1;
				c[(m_DS_FILTER_SIZE-1)/2+3]=-1;
				c[(m_DS_FILTER_SIZE-1)/2+4]=-1;
				scale=5;//6*5=30~32
			}
			else if(m_ds_level==4)
			{
				c[(m_DS_FILTER_SIZE-1)/2-6]=-1;
				c[(m_DS_FILTER_SIZE-1)/2-5]=-1;
				c[(m_DS_FILTER_SIZE-1)/2-4]=-1;
				c[(m_DS_FILTER_SIZE-1)/2-3]=-1;
				c[(m_DS_FILTER_SIZE-1)/2-2]=1;
				c[(m_DS_FILTER_SIZE-1)/2-1]=2;
				c[(m_DS_FILTER_SIZE-1)/2]=2; 
				c[(m_DS_FILTER_SIZE-1)/2+1]=2;
				c[(m_DS_FILTER_SIZE-1)/2+2]=1;
				c[(m_DS_FILTER_SIZE-1)/2+3]=-1;
				c[(m_DS_FILTER_SIZE-1)/2+4]=-1;
				c[(m_DS_FILTER_SIZE-1)/2+5]=-1;
				c[(m_DS_FILTER_SIZE-1)/2+6]=-1;
				scale=4;//8*4=32
			}
			return scale;
	}

	int configDDIFilterCoeffsHPF(int ddifilter,int c[])
	{
		int scale=1;
			for(int i=0;i<m_DS_FILTER_SIZE;i++)
			{
				c[i]=-1;
			}
			if(m_ds_level==1)
			{
				c[(m_DS_FILTER_SIZE-1)/2]=m_DS_FILTER_SIZE-1;//15
				scale=2; //2*15=30~32
			}
			else if((m_ds_level==2) || (m_ds_level==5))
			{
				c[(m_DS_FILTER_SIZE-1)/2-1]=2;
				c[(m_DS_FILTER_SIZE-1)/2]=m_DS_FILTER_SIZE-3-4; //8
				c[(m_DS_FILTER_SIZE-1)/2+1]=2;
				scale=3; //12*3=36~32
			}
			else if(m_ds_level==3)
			{
				c[(m_DS_FILTER_SIZE-1)/2-1]=4;
				c[(m_DS_FILTER_SIZE-1)/2]=m_DS_FILTER_SIZE-3-8; //15-11=4
				c[(m_DS_FILTER_SIZE-1)/2+1]=4;
				scale=3;//12*3=36~32
			}
			else if(m_ds_level==4)
			{
				c[(m_DS_FILTER_SIZE-1)/2-2]=2;
				c[(m_DS_FILTER_SIZE-1)/2-1]=2;
				c[(m_DS_FILTER_SIZE-1)/2]=m_DS_FILTER_SIZE-5-8; //15-13=2
				c[(m_DS_FILTER_SIZE-1)/2+1]=2;
				c[(m_DS_FILTER_SIZE-1)/2-2]=2;
				scale=3;//10*3=30~32
			}
			return scale;
	}

	int configDDIFilterCoeffsAA(int ddifilter,int c[])
	{
			if(m_ds_level==1)
			{
				c[(m_DS_FILTER_SIZE-1)/2]=16;//center value
			}
			else if((m_ds_level==2) || (m_ds_level==5))
			{
				c[(m_DS_FILTER_SIZE-1)/2-1]=4;//center value
				c[(m_DS_FILTER_SIZE-1)/2]=8;//center value
				c[(m_DS_FILTER_SIZE-1)/2+1]=4;//center value
			}
			else if(m_ds_level==3)
			{
				c[(m_DS_FILTER_SIZE-1)/2-1]=5;//center value
				c[(m_DS_FILTER_SIZE-1)/2]=6;//center value
				c[(m_DS_FILTER_SIZE-1)/2+1]=5;//center value
			}
			else if(m_ds_level==4)
			{
				c[(m_DS_FILTER_SIZE-1)/2-2]=2;//center value
				c[(m_DS_FILTER_SIZE-1)/2-1]=4;//center value
				c[(m_DS_FILTER_SIZE-1)/2]=4;//center value
				c[(m_DS_FILTER_SIZE-1)/2+1]=4;//center value
				c[(m_DS_FILTER_SIZE-1)/2+2]=2;//center value
			}
			int scale=2;
			return scale;
	}
	void configStatistics()
	{
		int cell_size;
		int sx=m_sizex-16;
		//•	The total number of statistical squares in the frame MUST be a multiplication of 8.
		//•	No more than 32 statistical boxes can fit into 14 rows.
		//•	The minimal number of pixel in statistical box is 512 (~24^2)

		if(0) //AREA CANNOT BE BELOW 512 PIX 
		{
			if(sx<=60)
			{
				cell_size=2;
			}
			else if(sx<=120)
			{
				cell_size=4;
			}
			else if(sx<=180)
			{
				cell_size=6;
			}
			else if(sx<=240)
			{
				cell_size=8;
			}
			else if(sx<=320)
			{
				cell_size=12;
			}
			else if(sx<=480)
			{
				cell_size=16;
			}
		}
		else if(sx<=640)
		{
			cell_size=24;
		}
		else if(sx<=960)
		{
			cell_size=32;
		}
		else if(sx<=1200)
		{
			cell_size=40;
		}
		else if(m_sizex<=1440)
		{
			cell_size=48;
		}
		else //if(m_sizex<=1920) CANNOT BE ABOVE 48!!
		{
			cell_size=64;
		}
	
		setDpeRegisterWithDecrement("dpe_alg_s_rect","length_x",cell_size);
		setDpeRegisterWithDecrement("dpe_alg_s_rect","length_y",cell_size);
	
		setDpeRegisterWithDecrement("dpe_alg_s_bbox","start_x",8);
		setDpeRegisterWithDecrement("dpe_alg_s_rect","start_y",8);
		setDpeRegisterWithDecrement("dpe_alg_s_bbox","end_y",m_sizey-16);
		int nx=(sx/cell_size);
		nx=nx-nx%8;
#if 1
		nx=1;
#endif
		setDpeRegisterWithDecrement("dpe_alg_s_grid","num_bin_x",nx);

		setStatisticsRangeYUV(0, 0.25,0.25,0.25,0.75,0.75,0.75);//SKIN
		setStatisticsRangeYUV(1, 0,0,0,0.3,1,1);//BLACKS
		setStatisticsRangeYUV(1, 0.7,0,0,1,.5,.5);//WHITES
	}

	void setStatisticsRangeYUV(int range_id, double y_min_f,double u_min_f,double v_min_f,double y_max_f,double u_max_f,double v_max_f)
	{
		char reg_name[256];
		sprintf(reg_name,"dpe_alg_s_yuv_max_rng%d",range_id);
		setDpeRegister(reg_name,"y",(int)(4095*y_max_f));
		setDpeRegister(reg_name,"u",(int)(15*u_max_f));
		setDpeRegister(reg_name,"v",(int)(15*v_max_f));
		sprintf(reg_name,"dpe_alg_s_yuv_min_rng%d",range_id);
		setDpeRegister(reg_name,"y",(int)(4095*y_min_f));
		setDpeRegister(reg_name,"u",(int)(15*u_min_f));
		setDpeRegister(reg_name,"v",(int)(15*v_min_f));
	}

	void configDecimation()
	{
		bool is_default;
		getConfigRegister("decimation", m_decimation,is_default);
		m_decimation_size;
		m_decimation_value=0;
		int decimation_vector[12];
		if(m_decimation>0)
		{
			int denum=m_decimation%10;
			int num=(m_decimation-denum)/10;
			m_decimation_size=denum;
			int vector_sum=0;
			for(int i=0; i<m_decimation_size; i++)
			{
				if((i+1)*num/denum>vector_sum)
				{
					decimation_vector[i]=1;
					m_decimation_value+=(1<<i);
					vector_sum++;
				}
				else
				{
					decimation_vector[i]=0;
				}
			}
			setDpeRegisterWithDecrement("dpe_alg_r0_decim_x","vector_size",m_decimation_size);
			setDpeRegister("dpe_alg_r0_decim_x","vector",m_decimation_value);
			setDpeRegisterWithDecrement("dpe_alg_r_decim_y","vector_size",m_decimation_size);
			setDpeRegister("dpe_alg_r_decim_y","vector",m_decimation_value);

		}
	}

	void GetDPEOutSizes()
	{
		//-----------------------------------------------------------------
		// calculate DPE output sizes and related registers

		int nWidthBeforeDecimation;
		if(m_bypass_active)
		{
			nWidthBeforeDecimation = m_inout_sizex;
		}
		else
		{
			nWidthBeforeDecimation = m_b01_sizex_before -m_DS_FILTER_EDGE;
		}

		// get sizes AFTER decimation

		int nWidthAfterDecimation;
		int nHightAfterDecimation;

		if(m_decimation>0)
		{
			nWidthAfterDecimation = GetDecimatedSize(nWidthBeforeDecimation, m_decimation_size, m_decimation_value);
			nHightAfterDecimation = GetDecimatedSize(m_sizey, m_decimation_size, m_decimation_value);
		}
		else
		{
			nWidthAfterDecimation = nWidthBeforeDecimation;
			nHightAfterDecimation = m_sizey;
		}

		// crop width to be divisible by 16

		int nWidthToIgnore = nWidthAfterDecimation%16;

		if(nWidthToIgnore > 0)
		{
			int nWidthToTake = nWidthAfterDecimation - nWidthToIgnore;

			setDpeRegisterWithDecrement("dpe_ctrl_out_data","hor_crop",nWidthToIgnore);
			setDpeRegisterWithDecrement("dpe_ctrl_out_data","hor",nWidthToTake);
			setDpeRegisterWithDecrement("dpe_ctrl_out_data","ver",nHightAfterDecimation);

			// set final number of pixels from DPE
			int nFinalNumPixels = nWidthToTake*nHightAfterDecimation;

			setDpeRegisterWithDecrement("dpe_ctrl_out_data_pixels","num",nFinalNumPixels);
		}
		else
		{
			int nFinalNumPixels = nWidthAfterDecimation*nHightAfterDecimation;
			setDpeRegisterWithDecrement("dpe_ctrl_out_data","hor",nWidthAfterDecimation);
			setDpeRegisterWithDecrement("dpe_ctrl_out_data","ver",nHightAfterDecimation);
			setDpeRegisterWithDecrement("dpe_ctrl_out_data_pixels","num",nFinalNumPixels);

		}
	}

//====================================================================================
//	
//	configMultiresolution	
//
//  Needs to be executed before other configurations...
//====================================================================================
	void readDisparityRange()
	{
		//m_output_min_disparity=getDpeRegister("dpe_alg_f_merge","omd");
		bool is_default;
		getConfigRegister("min_roi_disparity", m_roi_min_disparity,is_default);
		getConfigRegister("min_roi_disparity", m_roi_max_disparity,is_default);
		getConfigRegister("maxdisparity", m_max_disparity,is_default);
		getConfigRegister("mindisparity", m_min_disparity,is_default);
		int disparity_scale=0;
		getConfigRegister("disparity_scale", disparity_scale,is_default);
		m_output_min_disparity=min(m_roi_min_disparity,max(0,m_max_disparity-(240<<disparity_scale)));
		setDpeRegister("dpe_alg_f_merge","omd",m_output_min_disparity);
		if(m_min_disparity>m_roi_min_disparity)
		{
			m_roi_min_disparity=m_min_disparity;
		}
		if(m_max_disparity<m_roi_max_disparity)
		{
			m_roi_max_disparity=m_max_disparity;
		}

	}

	void configMultiresolution()
	{
		bool is_default;
		
		int multiresolution;
		getConfigRegister("multiresolution", multiresolution,is_default);
		if(multiresolution!=0)
		{
			m_bypass_active=0; //getDpeRegister("dpe_ddi_alg_bypass","active");
		}
		else
		{
			m_bypass_active=1;
		}
		setDpeRegister("dpe_ddi_alg_bypass","active",m_bypass_active);
		if(multiresolution!=0)
		{
			m_ds_level=multiresolution%10;
			m_us_level=(multiresolution-m_ds_level)/10;
			if((m_ds_level==0) && (m_us_level==0))
			{
				m_merge_active=0;
			}
			else if((m_ds_level>0) && (m_us_level==0))
			{

				m_merge_active=1;
			}
			else if((m_ds_level>0) && (m_us_level>0))
			{
				m_merge_active=2;
			}
			else if((m_ds_level==0) && (m_us_level>0))
			{
				m_merge_active=4;
				m_ds_level=4;
			}
			setDpeRegister("dpe_alg_f_merge","bypass",m_merge_active);

			setDpeRegisterWithDecrement( "dpe_ddi_alg_b01_general", "ds_level",m_ds_level);
			if(m_merge_active>1)
			{
				m_hybrid_row_active=1;
				setDpeRegisterWithDecrement( "dpe_ddi_alg_b00_general", "us_factor",m_us_level);
			}
			else
			{
				m_hybrid_row_active=0;
			}
		
			
		}
		else
		{
			if(!m_bypass_active)
			{
				m_ds_level=getDpeRegisterWithIncrement( "dpe_ddi_alg_b01_general", "ds_level");
				m_hybrid_row_active=getDpeRegister("dpe_alg_b","direct_hyb_row_en");
			}
			else
			{
				m_ds_level=1;
				m_hybrid_row_active=0;
			}
			if(m_hybrid_row_active>0)
			{
				m_us_level=getDpeRegisterWithIncrement("dpe_ddi_alg_b00_general", "us_factor");
			}
			else
			{
				m_us_level=1;
			}
		}
		setDpeRegister("dpe_alg_b","direct_hyb_row_en",m_hybrid_row_active);
	}

//====================================================================================
//	
//	VALIDATE	
//
//  Here I mainly set crops and calculate disparity machines
//====================================================================================
	void doValidate()
	{
		validateEdgeSize();
		validateDisparityCrop();
		validateSizeX();
		validateHands();
	}

	void validateHands()
	{
		bool is_default;
	

		m_num_blocks=1;
		getConfigRegister("hands_upsample", m_hands_upsample,is_default);
		if(m_hands_upsample>0)
		{
			if(validateOneHand(1,m_bA_l,m_bA_r,m_bA_t))
			{
				m_num_blocks++;
				if(validateOneHand(2,m_bB_l,m_bB_r,m_bB_t))
				{
					m_num_blocks++;
				}
			}
			else
			{
				if(validateOneHand(2,m_bA_l,m_bA_r,m_bA_t))
				{
					m_num_blocks++;
				}
			}
			
		}
	}

	bool validateOneHand(int handId,int& b_l,int& b_r,int& b_t)
	{
		bool is_default;
		char tmp[256];
		int hand_left;
		int hand_top;
		int hand_disparity;
		sprintf(tmp,"hand_%d_left",handId);
		getConfigRegister(tmp, hand_left,is_default);
		sprintf(tmp,"hand_%d_top",handId);
		getConfigRegister(tmp, hand_top,is_default);
		sprintf(tmp,"hand_%d_disparity",handId);
		getConfigRegister(tmp, hand_disparity,is_default);
		if((hand_left==0) && (hand_top==0))
		{
			b_l=b_t=b_r=0;
			return false;
		}
		b_l=hand_left+hand_disparity-m_HANDS_DISPARITY/2;
		b_r=hand_left;
		b_t=hand_top;
		return true;
	}


	void validateDisparityCrop()
	{
		m_b00_l=0;
		m_b00_r=0;
		m_b01_l=0;
		m_b01_r=0;
		if(m_min_disparity>0)
		{
			m_b01_l=m_min_disparity;
		}
		else
		{
			m_b01_r=m_min_disparity;
		}
		if(m_roi_min_disparity>0)
		{
			m_b00_l=m_roi_min_disparity;
		}
		else
		{
			m_b00_r=m_roi_min_disparity;
		}
	}

	void validateEdgeSize()
	{
		bool is_default;
		int edge_treatment;
		getConfigRegister("ds_dge_treatment", edge_treatment,is_default);
		if(edge_treatment==0)
		{
			m_US_LEFT_EDGE=4;
			m_US_RIGHT_EDGE=12;
			m_DS_FILTER_EDGE=16;
		}
		else
		{
			m_US_LEFT_EDGE=0;
			m_US_RIGHT_EDGE=0;
			m_DS_FILTER_EDGE=0;
		}
	}

	

	void validateSizeX()
	{
		m_ds_sz_delta=0;
		if(m_ds_level>1)
		{
			if(m_ds_level==5)
			{
				m_ds_sz_delta=(m_sizex-m_DS_FILTER_EDGE-(m_b01_l+m_b01_r))%(3*m_ALIGNMENT);
			}
			else
			{
				m_ds_sz_delta=(m_sizex-m_DS_FILTER_EDGE-(m_b01_l+m_b01_r))%(m_ds_level*m_ALIGNMENT);
			}
		}
		else
		{
			m_ds_sz_delta=(m_sizex-(m_b01_l+m_b01_r))%(m_ds_level*m_ALIGNMENT);
		}
		//m_us_sz_delta=((m_sizex-(m_b00_l+m_b00_r))*m_us_level)%m_ALIGNMENT;
		m_us_sz_delta=(m_sizex-(m_b00_l+m_b00_r))%m_ALIGNMENT;
		while(m_ds_sz_delta>m_us_sz_delta)
		{
			m_us_sz_delta+=m_ALIGNMENT;
		}

		if(m_ds_level==5)
		{
			m_b01_sizex=(m_sizex - m_DS_FILTER_EDGE-m_ds_sz_delta-(m_b01_l+m_b01_r))*2/3;
		}
		else
		{
			m_b01_sizex=(m_sizex - m_DS_FILTER_EDGE-m_ds_sz_delta-(m_b01_l+m_b01_r))/m_ds_level;
		}
		m_b01_sizex_before=(m_sizex-m_ds_sz_delta-(m_b01_l+m_b01_r));
		if(m_us_level==5)
		{
			m_b00_sizex=(m_sizex-m_US_LEFT_EDGE-m_US_RIGHT_EDGE-m_us_sz_delta-(m_b00_l+m_b00_r))*3/2;
		}
		else
		{
			m_b00_sizex=(m_sizex-m_US_LEFT_EDGE-m_US_RIGHT_EDGE-m_us_sz_delta-(m_b00_l+m_b00_r))*m_us_level;
		}
		m_b00_sizex_before=(m_sizex-m_US_LEFT_EDGE-m_us_sz_delta-(m_b00_l+m_b00_r));
		if(m_b00_sizex+m_b01_sizex>1900) //ALLOW MAX LINE LENGTH 1900
		{
			int t=m_b00_sizex+m_b01_sizex-1900;
			t=t-t%m_ALIGNMENT;
			if(m_us_level==5)
			{
				m_us_sz_delta+=t/3*2;
				m_b00_sizex=(m_sizex-m_US_LEFT_EDGE-m_US_RIGHT_EDGE-m_us_sz_delta-(m_b00_l+m_b00_r))*3/2;
				m_b00_sizex_before=(m_sizex-m_US_LEFT_EDGE-m_us_sz_delta-(m_b00_l+m_b00_r));
				m_b00_l+=t/3;
				m_b00_r+=t/3;
			}
			else
			{
				int us_center;
				bool is_default;
				bool is_present;

				is_present=getConfigRegister("us_center", us_center,is_default);
				m_us_sz_delta+=t/m_us_level;
				m_b00_sizex=(m_sizex-m_US_LEFT_EDGE-m_US_RIGHT_EDGE-m_us_sz_delta-(m_b00_l+m_b00_r))*m_us_level;
				int b00_size_prev=m_b00_sizex_before;
				m_b00_sizex_before=(m_sizex-m_US_LEFT_EDGE-m_us_sz_delta-(m_b00_l+m_b00_r));
				if(us_center==0)
				{
					m_b00_l+=t/m_us_level/2;
					m_b00_r+=t/m_us_level/2;
				}
				else
				{
					m_b00_l+=t/2/m_us_level+us_center-b00_size_prev/2;
					m_b00_r+=t/2/m_us_level+us_center-b00_size_prev/2;

					int dd;
					dd=m_b00_l;
					if(dd>m_b00_r) dd=m_b00_r;
					if(dd<m_US_LEFT_EDGE*m_us_level)
					{
						dd=m_US_LEFT_EDGE*m_us_level-dd;
						m_b00_l+=dd;
						m_b00_r+=dd;
					}
					dd=m_b00_l;
					if(dd<m_b00_r) dd=m_b00_r;
					if(dd+m_b00_sizex_before+m_US_RIGHT_EDGE*m_us_level>b00_size_prev)
					{
						dd=b00_size_prev-m_b00_sizex_before-m_US_RIGHT_EDGE*m_us_level;
						m_b00_l-=dd;
						m_b00_r-=dd;
					}

				}



			}
		}
		
		

		if(m_bypass_active)
		{
			m_inout_sizex=m_sizex;
		}
		else
		{
			if(m_hybrid_row_active)
			{
				m_inout_sizex=m_b01_sizex + m_b00_sizex;
			}
			else
			{
				m_inout_sizex=m_b01_sizex;
			}
		}
		int res=0; //for debugging only to put breakpoint
	}

//====================================================================================
//	
//	WRITE 
//
//  Here I mainly set sizes of blocks
//====================================================================================

	void writeToDB()
	{
		setB0();
		
		if(m_num_blocks>1)
		{
			setBA();
		}
		if(m_num_blocks>2)
		{
			setBB();
		}
		//setDpeRegisterWithDecrement("dpe_ddi_alg_direct_size", "y", m_sizey+(m_num_blocks-1)*100);
		setDpeRegisterWithDecrement("dpe_alg_b", "num_blocks",m_num_blocks);
		readODP();
		setDepth();
		setPostprocess();
	}

	void setBA()
	{
		setDpeRegister("dpe_ddi_alg_box_a_gen","enable", 1);
		setDpeRegisterWithDecrement("dpe_ddi_alg_box_a_gen","us_factor", m_hands_upsample);
		setDpeRegisterWithDecrement("dpe_ddi_alg_box_a_crop","start_y", m_bA_t);
		setDpeRegisterWithDecrement("dpe_ddi_alg_box_a_crop","r_start_x", m_bA_r);
		setDpeRegisterWithDecrement("dpe_ddi_alg_box_a_crop","l_start_x", m_bA_l);
		setDpeRegisterWithDecrement("dpe_alg_b1","ver_size", 100);
		uint32 m1,m2,m3;
		int maxm;
		getMachineEnable(0,m_HANDS_DISPARITY*m_hands_upsample,m1,m2,m3,maxm);
		setDpeRegister("dpe_alg_cf1_mach_disp_0", "en", m1);
		setDpeRegister("dpe_alg_cf1_mach_disp_1", "en", m2);
		setDpeRegister("dpe_alg_cf1_mach_disp_2", "en", m3);
		//m_HANDS_DISPARITY
	}

	void setBB()
	{
		setDpeRegister("dpe_ddi_alg_box_b_gen","enable", 1);
		setDpeRegisterWithDecrement("dpe_ddi_alg_box_b_gen","us_factor", m_hands_upsample);
		setDpeRegisterWithDecrement("dpe_ddi_alg_box_b_crop","start_y", m_bB_t);
		setDpeRegisterWithDecrement("dpe_ddi_alg_box_b_crop","r_start_x", m_bB_r);
		setDpeRegisterWithDecrement("dpe_ddi_alg_box_b_crop","l_start_x", m_bB_l);
		setDpeRegisterWithDecrement("dpe_alg_b2","ver_size", 100);
		uint32 m1,m2,m3;
		int maxm;
		getMachineEnable(0,m_HANDS_DISPARITY*m_hands_upsample,m1,m2,m3,maxm);
		setDpeRegister("dpe_alg_cf2_mach_disp_0", "en", m1);
		setDpeRegister("dpe_alg_cf2_mach_disp_1", "en", m2);
		setDpeRegister("dpe_alg_cf2_mach_disp_2", "en", m3);
		//m_HANDS_DISPARITY
	}


	void setB0()
	{
		setDpeRegisterWithDecrement("dpe_ddi_in_out_img_size", "hor", m_inout_sizex);
		setDpeRegisterWithDecrement("dpe_alg_b0", "ver_size", m_sizey);

		setDpeRegisterWithDecrement("dpe_ddi_alg_in", "hor_size", m_sizex);
		setDpeRegisterWithDecrement("dpe_ddi_alg_in", "ver_size", m_sizey);
		setDpeRegisterWithDecrement("dpe_ddi_alg_direct_size", "y", m_sizey);
		
		
		if(!m_bypass_active)
		{
			setB0_B01();
			if(!m_hybrid_row_active)
			{
				setDpeRegister("dpe_ddi_alg_b00_general", "enable", 0);
			}
			else
			{
				setDpeRegister("dpe_ddi_alg_b00_general", "enable", 1);
				if(m_ds_level==5)
				{
					setDpeRegister("dpe_alg_f_merge","toler",1+2);
				}
				else
				{
					setDpeRegister("dpe_alg_f_merge","toler",1+m_ds_level);
				}
				setB0_B00();
			}
		}
		enableMachinesPerBlockB0();
	}

	void setB0_B01()
	{
			setDpeRegisterWithDecrement("dpe_ddi_alg_b01_general", "size_x", m_b01_sizex_before);
			setDpeRegisterWithDecrement("dpe_alg_b01", "row_width", m_b01_sizex);
			setDpeRegister("dpe_ddi_alg_b01_start_x", "r", 0+m_b01_r);
			setDpeRegister("dpe_ddi_alg_b01_start_x", "l", 0+m_b01_l);
	}

	void setB0_B00()
	{
			setDpeRegisterWithDecrement("dpe_ddi_alg_b00_general", "size_x", m_b00_sizex_before);
			setDpeRegisterWithDecrement("dpe_alg_b00", "row_width", m_b00_sizex);
			setDpeRegister("dpe_ddi_alg_b00_start_x", "r", m_US_LEFT_EDGE+m_b00_r);
			setDpeRegister("dpe_ddi_alg_b00_start_x", "l", m_US_LEFT_EDGE+m_b00_l);
	}

	void enableMachinesPerBlockB0()
	{
		uint32 m1 = 0;
		uint32 m2 = 0;
		uint32 m3 = 0;
		int mMaxDS, mMaxROI;
		int min_disp=0;
		int max_disp=143;
		//{"L2Rconsistency.nROIdispMin",				0, 1023, 0},
		//{"L2Rconsistency.nROIdispMax",				0, 1023, 143},
		if(m_hybrid_row_active)
		{
			if(m_ds_level==5)
			{
				getMachineEnable(m_min_disparity/3*2,m_max_disparity/3*2,m1,m2,m3,mMaxDS);
			}
			else
			{
				getMachineEnable(m_min_disparity/m_ds_level,m_max_disparity/m_ds_level,m1,m2,m3,mMaxDS);
			}
			if((m_disable_machines==2)||(m_disable_machines==3))//0-all enabled, 1-ROI disabled, 2-DS disabled, 3-all disabled 
			{
				m1=0;
				m2=0;
				m3=0;
				mMaxDS=0;
			}
			setDpeRegister("L2Rconsistency","MergeUseMaxDsMachine",mMaxDS);
			setDpeRegister("dpe_alg_cf01_mach_disp_0", "en", m1);
			setDpeRegister("dpe_alg_cf01_mach_disp_1", "en", m2);
			setDpeRegister("dpe_alg_cf01_mach_disp_2", "en", m3);

			if(m_us_level==5)
			{
				if(m_output_min_disparity==0)
				{
					min_disp=m_roi_min_disparity*3/2;
				}
				else
				{
					min_disp=m_output_min_disparity*3/2;
				}
				max_disp=m_max_disparity*3/2;
				if(max_disp>min_disp+144)
				{
					max_disp=min_disp+144;
				}
			}
			else
			{
				if(m_output_min_disparity==0)
				{
					min_disp=m_roi_min_disparity*m_us_level;
				}
				else
				{
					min_disp=m_output_min_disparity*m_us_level;
				}
				max_disp=m_max_disparity*m_us_level;
				if(m_us_level>1)
				{
					if(max_disp>min_disp+144)
					{
						max_disp=min_disp+144;
					}
				}
			}
			getMachineEnable(min_disp,max_disp,m1,m2,m3,mMaxROI);
			if((m_disable_machines==1)||(m_disable_machines==3))//0-all enabled, 1-ROI disabled, 2-DS disabled, 3-all disabled 
			{
				m1=0;
				m2=0;
				m3=0;
				mMaxROI=0;
			}
			if(m3>255) 
			{
				m3=255;
			}
			//setDpeRegister("L2Rconsistency","MergeUseMaxRoiMachine",mMaxROI);
			setDpeRegister("dpe_alg_cf0_mach_disp_0", "en", m1);
			setDpeRegister("dpe_alg_cf0_mach_disp_1", "en", m2);
			setDpeRegister("dpe_alg_cf0_mach_disp_2", "en", m3);
			if(m_us_level==5)
			{
				m_roi_max_disparity=m_roi_min_disparity+mMaxROI*2/3;
			}
			else
			{
				m_roi_max_disparity=m_roi_min_disparity+mMaxROI/m_us_level;
			}
			setDpeRegister("L2Rconsistency","nROIdispMin",m_roi_min_disparity);
			setDpeRegister("L2Rconsistency","nROIdispMax",m_roi_max_disparity);
		}
		else
		{
			if(m_ds_level==5)
			{
				getMachineEnable(m_min_disparity/3*2,m_max_disparity/3*2,m1,m2,m3,mMaxDS);
			}
			else
			{
				getMachineEnable(m_min_disparity/m_ds_level,m_max_disparity/m_ds_level,m1,m2,m3,mMaxDS);
			}
			if(m_disable_machines==3)
			{
				m1=0;
				m2=0;
				m3=0;
			}
			//setDpeRegister("L2Rconsistency","MergeUseMaxRoiMachine",mMaxDS);
			setDpeRegister("dpe_alg_cf0_mach_disp_0", "en", m1);
			setDpeRegister("dpe_alg_cf0_mach_disp_1", "en", m2);
			setDpeRegister("dpe_alg_cf0_mach_disp_2", "en", m3);

			
		}

		//{"L2Rconsistency.nROIdispMin",				0, 1023, 0},
		//{"L2Rconsistency.nROIdispMax",				0, 1023, 143},
	}

	void getMachineEnable(int min_disparity, int max_disparity, uint32& m1,uint32& m2,uint32& m3, int& mMax)
	{
		int delta=max_disparity-min_disparity;
		if(delta>144)
		{
			delta=144;
		}
		mMax=delta;
		if(delta<=0)
		{
			getMachineEnableField(0, m1);
			getMachineEnableField(0, m2);
			getMachineEnableField(0, m3);
		}
		else if(delta<64)
		{
			getMachineEnableField(delta+1, m1);
			getMachineEnableField(0, m2);
			getMachineEnableField(0, m3);
		}
		else if(delta<128)
		{
			getMachineEnableField(64, m1);
			getMachineEnableField(delta-64+1, m2);
			getMachineEnableField(0, m3);
		}
		else
		{
			getMachineEnableField(64, m1);
			getMachineEnableField(64, m2);
			getMachineEnableField(delta-128+1, m3);
		}
	}
	void getMachineEnableField(int delta, uint32& m)
	{
		m=0;
		int delta2=(delta+1)/2;
		if(delta2>32) delta2=32;
		if(delta2<0) delta2=0;
		//for(int i=0; i<delta2-1; i++)
		for(int i=0; i<delta2; i++)
		{
			m+=(1<<i);
		}
	}
	void readODP()
	{
		//[CVE_PostProcess]
		//nOutOptFactor=26848714
		//nOutDispOffset=15311
		//nPLY_OptCenX=30260
		//nPLY_OptCenY=25555
		//nPLY_FocLen=133019

		//fc_left=938.272374
		//cc_left_x=637.000000
		//cc_left_y=480.000000
		float fc_right=938.272374f;
		float cc_right_x=637.000000f;
		float cc_right_y=480.000000f;
		float T=195.866812f;
		//Nx=1224.000000
		//Ny=920.000000

		//int focal_int=getDpeRegister("CVE_PostProcess","nPLY_FocLen");
		//int optcenx_int=getDpeRegister("CVE_PostProcess","nPLY_OptCenX");
		//int optceny_int=getDpeRegister("CVE_PostProcess","nPLY_OptCenY");

		
		mOpticalData.FocalLengthBaseRight.X() =	fc_right;
		mOpticalData.OpticalCenterBaseRight.X()=cc_right_x;
		mOpticalData.OpticalCenterBaseRight.Y()=cc_right_y;
		mOpticalData.OpticalCenterBaseLeft =	mOpticalData.OpticalCenterBaseRight;
		mOpticalData.FocalLengthBaseLeft =	mOpticalData.FocalLengthBaseRight;
		mOpticalData.TranslationRectifiedLeft.X() = T;
	}

	void setDepth()
	{
		float iDownScaleFactor=1.0;
		short iHorizPadding = 0;
        short iVerticalPadding = 0;


		InuCommon::DPEDepthOpticalData myData(mOpticalData,iDownScaleFactor,iHorizPadding,iVerticalPadding);
		setDpeRegister("dpe_alg_r0_trans","optics_factor",(int)(myData.mOpticalFactor));
		setDpeRegister("dpe_alg_r0_disp2depth","disp_offset",(int)(myData.mDisparityOffset));
		setDpeRegister("dpe_alg_r0_disp2depth","z_min",(int)(myData.mZmin));
	
		//setDpeRegister("CVE_PostProcess","nOutOptFactor",(int)(myData.mOpticalFactor/16));
		//setDpeRegister("CVE_PostProcess","nOutDispOffset",(int)(myData.mDisparityOffset/16));

		setDpeRegister("CVE_PostProcess","nPLY_OptCenX",(int)(100*myData.OpticalCenterBaseRight.X()));
		setDpeRegister("CVE_PostProcess","nPLY_OptCenY",(int)(100*myData.OpticalCenterBaseRight.Y()));
		setDpeRegister("CVE_PostProcess","nPLY_FocLen",(int)(100*myData.FocalLengthBaseRight.X()));

	}
	void setPostprocess()
	{
		return;
		setDpeRegister("CVE_PostProcess","nGuiMinDisp",0);
		setDpeRegister("CVE_PostProcess","nGuiMaxDisp",143); //240?
		if(m_ds_level>0)
		{
			setDpeRegister("CVE_PostProcess","nGuiMaxDisp",240); 
		}
		setDpeRegister("CVE_PostProcess","nIsPLYFromDispOrDepth",0);

		setDpeRegister("dpe_alg_r_disp2depth_mode","depth_16bit_en",1);  //depth 16 bit
		//setDpeRegister("dpe_alg_r_disp2depth_mode","depth_16bit_en",0);  //depth 14 bit + conf
		

	}
	
//====================================================================================
//	
//	TEST OVERRIDE
//
//  Here I override registers for test purposes. Should do nothing in production.
//====================================================================================


	void testOverride()
	{

	}

	//---------------------------------------------------------------------------------
	//

	int GetDecimatedSize(int nSizeIn, int nDecVectorSize, int nDecPattern)
	{
	int nNumPixelOut = 0;
	int k;

	int nCurPattern = nDecPattern;
	for(k=0;k<nDecVectorSize;k++)
	{
		nNumPixelOut = nNumPixelOut  + nCurPattern&1;

		nCurPattern >>= 1;
	}

	int nNumFullVector = nSizeIn/nDecVectorSize;
	int nOutSize = nNumFullVector * nNumPixelOut;
	int nRemindVec = nSizeIn - nDecVectorSize * nNumFullVector;

	nCurPattern = nDecPattern;
	for(k=0;k<nRemindVec;k++)
	{
		nOutSize = nOutSize  + nCurPattern&1;

		nCurPattern >>= 1;
	}

	return nOutSize;
}

};

#endif