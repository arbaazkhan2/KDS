% Intrinsic and Extrinsic Camera Parameters


%-- Focal length:
fc = [ 2923.082567669489400 ; 3034.835440917774600 ];

%-- Principal point:
cc = [ 1786.716229511124500 ; 1615.123620925456200 ];

%-- Skew coefficient:
alpha_c = 0.000000000000000;

%-- Distortion coefficients:
kc = [ 0.038739697912235 ; -0.244906975528383 ; 0.011326718576945 ; 0.005995629564705 ; 0.000000000000000 ];

%-- Focal length uncertainty:
fc_error = [ 35.861239822550985 ; 42.135794553869360 ];

%-- Principal point uncertainty:
cc_error = [ 58.709367758446170 ; 67.506665396414192 ];

%-- Skew coefficient uncertainty:
alpha_c_error = 0.000000000000000;

%-- Distortion coefficients uncertainty:
kc_error = [ 0.069498161521798 ; 0.215146230745530 ; 0.008337740215246 ; 0.006718536690828 ; 0.000000000000000 ];

%-- Image size:
nx = 3648;
ny = 2736;


%-- Various other variables :
%-- Those variables are used to control which intrinsic parameters should be optimized

n_ima = 18;						% Number of calibration images
est_fc = [ 1 ; 1 ];					% Estimation indicator of the two focal variables
est_aspect_ratio = 1;				% Estimation indicator of the aspect ratio fc(2)/fc(1)
center_optim = 1;					% Estimation indicator of the principal point
est_alpha = 0;						% Estimation indicator of the skew coefficient
est_dist = [ 1 ; 1 ; 1 ; 1 ; 0 ];	% Estimation indicator of the distortion coefficients


%-- Extrinsic parameters:
%-- The rotation (omc_kk) and the translation (Tc_kk) vectors for every calibration image and their uncertainties

%-- Image #1:
omc_1 = [ 2.129434e+00 ; 2.138366e+00 ; -1.976888e-01 ];
Tc_1  = [ -2.530870e+02 ; -4.318178e+02 ; 1.158278e+03 ];
omc_error_1 = [ 2.071224e-02 ; 2.376084e-02 ; 4.470209e-02 ];
Tc_error_1  = [ 2.392556e+01 ; 2.569408e+01 ; 2.001296e+01 ];

%-- Image #2:
omc_2 = [ 2.177995e+00 ; 2.175035e+00 ; -8.670704e-02 ];
Tc_2  = [ -2.600632e+02 ; -4.260026e+02 ; 1.113050e+03 ];
omc_error_2 = [ 2.192509e-02 ; 2.420449e-02 ; 4.749729e-02 ];
Tc_error_2  = [ 2.302728e+01 ; 2.486921e+01 ; 2.031353e+01 ];

%-- Image #3:
omc_3 = [ 1.724969e+00 ; 1.831030e+00 ; -7.832426e-01 ];
Tc_3  = [ -2.392441e+02 ; -3.614979e+02 ; 1.270513e+03 ];
omc_error_3 = [ 1.554969e-02 ; 2.218801e-02 ; 2.871598e-02 ];
Tc_error_3  = [ 2.603897e+01 ; 2.799387e+01 ; 1.643096e+01 ];

%-- Image #4:
omc_4 = [ 1.810648e+00 ; 1.909244e+00 ; -6.838454e-01 ];
Tc_4  = [ -2.394639e+02 ; -4.044405e+02 ; 1.200438e+03 ];
omc_error_4 = [ 1.541565e-02 ; 2.202080e-02 ; 3.061795e-02 ];
Tc_error_4  = [ 2.480388e+01 ; 2.646294e+01 ; 1.661561e+01 ];

%-- Image #5:
omc_5 = [ 1.878671e+00 ; 2.009318e+00 ; -5.921063e-01 ];