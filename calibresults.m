% Intrinsic and Extrinsic Camera Parameters


%-- Focal length:
fc = [ 3055.405691159269300 ; 3015.296945830715700 ];

%-- Principal point:
cc = [ 1510.506814935316500 ; 1292.554499968259700 ];

%-- Skew coefficient:
alpha_c = 0.000000000000000;

%-- Distortion coefficients:
kc = [ 0.008880371756185 ; -0.208948440284388 ; 0.008222054334677 ; -0.011325621742207 ; 0.000000000000000 ];

%-- Focal length uncertainty:
fc_error = [ 100.286753906020440 ; 99.102746956248140 ];

%-- Principal point uncertainty:
cc_error = [ 129.471997014952900 ; 100.475469970254860 ];

%-- Skew coefficient uncertainty:
alpha_c_error = 0.000000000000000;

%-- Distortion coefficients uncertainty:
kc_error = [ 0.140547898935574 ; 0.445046628574472 ; 0.010058644365397 ; 0.015769869021601 ; 0.000000000000000 ];

%-- Image size:
nx = 3648;
ny = 2736;


%-- Various other variables (may be ignored if you do not use the Matlab Calibration Toolbox):


n_ima = 6;						% Number of calibration images
est_fc = [ 1 ; 1 ];					% Estimation indicator of the two focal variables
est_aspect_ratio = 1;				% Estimation indicator of the aspect ratio fc(2)/fc(1)
center_optim = 1;					% Estimation indicator of the principal point
est_alpha = 0;						% Estimation indicator of the skew coefficient
est_dist = [ 1 ; 1 ; 1 ; 1 ; 0 ];	% Estimation indicator of the distortion coefficients


%-- Extrinsic parameters:
%-- The rotation (omc_kk) and the translation (Tc_kk) vectors for every calibration image and their uncertainties

%-- Image #1:
omc_1 = [ -2.190835e+00 ; -2.195107e+00 ; -4.361417e-02 ];
Tc_1  = [ -1.734433e+02 ; -2.928813e+02 ; 1.052399e+03 ];
omc_error_1 = [ 2.584316e-02 ; 3.019486e-02 ; 6.594976e-02 ];
Tc_error_1  = [ 4.524743e+01 ; 3.532413e+01 ; 3.780740e+01 ];

%-- Image #2:
omc_2 = [ -2.259086e+00 ; -2.210509e+00 ; -1.422452e-01 ];
Tc_2  = [ -1.597683e+02 ; -2.999837e+02 ; 1.096075e+03 ];
omc_error_2 = [ 2.664563e-02 ; 3.188235e-02 ; 6.828147e-02 ];
Tc_error_2  = [ 4.727131e+01 ; 3.666491e+01 ; 3.965977e+01 ];

%-- Image #3:
omc_3 = [ 2.084923e+00 ; 1.966081e+00 ; 5.668037e-01 ];
Tc_3  = [ 6.518361e+01 ; -3.151849e+02 ; 1.114383e+03 ];
omc_error_3 = [ 3.812267e-02 ; 2.669346e-02 ; 6.086503e-02 ];
Tc_error_3  = [ 4.827883e+01 ; 3.673163e+01 ; 4.307245e+01 ];

%-- Image #4:
omc_4 = [ -1.740136e+00 ; -1.785251e+00 ; 6.726197e-01 ];
Tc_4  = [ -2.871695e+01 ; -3.340440e+02 ; 1.514544e+03 ];
omc_error_4 = [ 3.394805e-02 ; 3.009160e-02 ; 5.121486e-02 ];
Tc_error_4  = [ 6.485803e+01 ; 4.992221e+01 ; 4.357943e+01 ];

%-- Image #5:
omc_5 = [ 1.842261e+00 ; 1.965732e+00 ; -7.719490e-01 ];
Tc_5  = [ -1.160645e+02 ; -3.343330e+02 ; 1.341508e+03 ];
omc_error_5 = [ 2.427640e-02 ; 3.564756e-02 ; 5.596375e-02 ];
Tc_error_5  = [ 5.727442e+01 ; 4.405729e+01 ; 3.794249e+01 ];

%-- Image #6:
omc_6 = [ -1.844238e+00 ; -1.747007e+00 ; -5.095267e-01 ];
Tc_6  = [ -1.806152e+02 ; -2.356041e+02 ; 1.035207e+03 ];
omc_error_6 = [ 2.158002e-02 ; 3.230387e-02 ; 5.394405e-02 ];
Tc_error_6  = [ 4.452098e+01 ; 3.460459e+01 ; 3.878120e+01 ];

