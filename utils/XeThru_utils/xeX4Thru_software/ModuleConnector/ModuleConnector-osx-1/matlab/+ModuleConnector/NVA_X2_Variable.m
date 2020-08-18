classdef NVA_X2_Variable
    % NVA_X2_Variable is the counterpart of < NVA_X2-registers-id.h > and
    % <VA_X2-variables-id.h> firmware, which contains the IDs used to
    % access the look up table with register definitions for each register
    % field and each library variable respectively.
    %
    % Preamble NVA_X2_REGFIELD_ indicates X2 register field.
    % Preamble NVA_X2_VAR_ indicates module firmware variable.
    %
    % Property value contains
    % {mapping_value, variable_type, read/write-permission}
    %
    %
    
    % Registers
    properties (Constant)
        NVA_X2_REGFIELD_ChipID                      = { 2,'REGFIELD','R'}
        NVA_X2_REGFIELD_Downsampling                = {14,'REGFIELD','W'}
        NVA_X2_REGFIELD_PulsesPerStep               = {21,'REGFIELD','W'}
        NVA_X2_REGFIELD_DACFirstIterationSetupTime  = {22,'REGFIELD','R'}
        NVA_X2_REGFIELD_DACFirstStepSetupTime       = {23,'REGFIELD','R'}
        NVA_X2_REGFIELD_DACRegularStepSetupTime     = {24,'REGFIELD','R'}
        NVA_X2_REGFIELD_DACLastIterationHoldTime    = {25,'REGFIELD','R'}
        NVA_X2_REGFIELD_DACLastStepHoldTime         = {26,'REGFIELD','R'}
        NVA_X2_REGFIELD_DACRegularStepHoldTime      = {27,'REGFIELD','R'}
        NVA_X2_REGFIELD_DACMax                      = {32,'REGFIELD','W'}
        NVA_X2_REGFIELD_DACMin                      = {33,'REGFIELD','W'}
        NVA_X2_REGFIELD_DACStepCoarse               = {34,'REGFIELD','W'}
        NVA_X2_REGFIELD_DACStepFine                 = {35,'REGFIELD','W'}
        NVA_X2_REGFIELD_Iterations                  = {36,'REGFIELD','W'}
        NVA_X2_REGFIELD_Sweeping                    = {37,'REGFIELD','R'}
        NVA_X2_REGFIELD_PGSelect                    = {39,'REGFIELD','W'}
        NVA_X2_REGFIELD_MClkDiv                     = {52,'REGFIELD','W'}
        NVA_X2_REGFIELD_PRFDivide                   = {61,'REGFIELD','W'}
        NVA_X2_REGFIELD_StaggeredPRFEnable          = {63,'REGFIELD','W'}
        NVA_X2_REGFIELD_StaggeredPRFDelay           = {64,'REGFIELD','W'}
        NVA_X2_REGFIELD_LFSRTaps                    = {65,'REGFIELD','W'}
        NVA_X2_REGFIELD_PGMeasResult                = {88,'REGFIELD','R'}
        NVA_X2_REGFIELD_SWSweepMode                 = {95,'REGFIELD','W'}
    end
    
    %%
    % Firmware counterpart < NVA_X2-variables-id.h >
    % NVA_X2 integer variables (NVA_X2_VAR_int_)
    properties (Constant)
        %
        % Number of radar frames to stitch together.
        %
        NVA_X2_VAR_FRAME_STITCH             = {0,'INT','W'}
        NVA_X2_VAR_PIPE_LINING             = {1,'INT','W'}
    end
    
    %%
    % Firmware counterpart < NVA_X2-variables-id.h >
    % NVA_X2 floating point variables (NVA_X2_VAR_float_)
    properties (Constant)
        %
        % Sample delay in seconds.
        %
        NVA_X2_VAR_SAMPLE_DELAY             = {0,'FLOAT','W'}
        %
        % Read only variable that fetches the value of SamplesPerSecond
        % from the timing measurement data struct.
        %
        NVA_X2_VAR_SAMPLES_PER_SECOND       = {1,'FLOAT','R'}
    end
    
    
    methods
        %function var = NVA_X2_Variable()
        %
        %end
    end
end

