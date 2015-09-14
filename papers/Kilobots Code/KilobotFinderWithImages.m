% http://www.mathworks.com/matlabcentral/answers/195257-how-can-i-detect-round-objects-and-remove-other-objects-in-an-image-using-matlab
% ould use blob properties


% ******************Loading input Image**********************************
%fn = 'BadResult.png';  % robots are 14 px in diameter in this
%fn = 'TestImage2.png';
fn = 'FailImage.png';
originalImage = imread(fn);
% Crop the image to use ROI.
originalImage = imcrop(originalImage,[345 60 1110 850]);
% make grayscale.
originalImage = rgb2gray(originalImage);

%originalImage = originalImage(33:33+550,250:250+810,:);  %ROI, you can use imrect to draw a rectangle using your mouse.

% Too easy, from
% http://www.mathworks.com/help/images/examples/detect-and-measure-circular-objects-in-an-image.tml
tic
BW = im2bw(originalImage, 0.15);  %threshold the image to remove shadows (and only show dark parts of kilobots)
    [centers, radii] = imfindcircles(BW,[12 22],'ObjectPolarity','dark','Sensitivity',0.94 );
%[centers, radii] = imfindcircles(originalImage,[12 19],'ObjectPolarity','dark','Sensitivity',0.91 );
toc
M = mean(centers);
V = var(centers);
C = cov(centers);

%figure(1);BW = im2bw(originalImage, 0.19); imshow(BW);
imshow(originalImage)
%imline measure size
%Draw the circles
h = viscircles(centers,radii);
hold on
plot(M(1,1) , M(1,2),'*','Markersize',16)
plot_gaussian_ellipsoid(M,C);
plot(centers(:,1),centers(:,2),'+','Markersize',16)
hold off


return


%%
I = rgb2gray(imread('FailImage.png'));
J = imadjust(I);
imshow(I)
figure, imshow(J)

% Could I find the holes in an image, and fill them?
%imfill(im,'holes'); 

%% http://www.mathworks.com/help/images/ref/imfill.html
fn = 'FailImage.png';
originalImage = imread(fn);
% Crop the image to use ROI.
originalImage = imcrop(originalImage,[345 60 1110 850]);
% make grayscale.
I = rgb2gray(originalImage);
I2 = imfill(I,'holes');
figure, imshow(I), figure, imshow(255-I)

