
originalImage = imread('testImage2.png');  % Load a jpeg image

I = rgb2hsv(originalImage);

% Define thresholds for channel 1 based on histogram settings
channel1Min = 0.065;
channel1Max = 0.567;

% Define thresholds for channel 2 based on histogram settings
channel2Min = 0.288;
channel2Max = 1.000;

% Define thresholds for channel 3 based on histogram settings
channel3Min = 0.400;
channel3Max = 1.000;

% Create mask based on chosen histogram thresholds
BW = (I(:,:,1) >= channel1Min ) & (I(:,:,1) <= channel1Max) & ...
    (I(:,:,2) >= channel2Min ) & (I(:,:,2) <= channel2Max) & ...
    (I(:,:,3) >= channel3Min ) & (I(:,:,3) <= channel3Max);

  %threshold the image to remove shadows (and only show dark parts of kilobots)
    [centers, radii] = imfindcircles(BW,[10 19],'ObjectPolarity','bright','Sensitivity',0.92 );
    [obstacles, radii2]= imfindcircles(BW,[100 120],'ObjectPolarity','bright','Sensitivity',0.92 );
        % %Mean
    M = mean(centers);
    %Variance
    V = var(centers);
    %Covariance
    C = cov(centers);
    
    imshow(originalImage);
    h = viscircles(centers,radii,'EdgeColor','b');
