%%%%%%%%% By Shiva Shahrokhi Dec 2015-Jan 2016: This code takes a snapshot of the
%%%%%%%%% webcam, then process it to find obstacles, gives that map to MDP
%%%%%%%%% and gets the result and draw the gradients. 

webcamShot = true;
if webcamShot
cam = webcam(2);
 originalImage = snapshot(cam);
 img = imcrop(originalImage,[345 60 1110 850]);

else
img = imread('testImage12.png');  % Load a jpeg image
end
% Convert RGB image to chosen color space
I = rgb2hsv(img);

% Define thresholds for channel 1 based on histogram settings
channel1Min = 0.847;
channel1Max = 0.946;

% Define thresholds for channel 2 based on histogram settings
channel2Min = 0.174;
channel2Max = 1.000;

% Define thresholds for channel 3 based on histogram settings
channel3Min = 0.657;
channel3Max = 1.000;

% Create mask based on chosen histogram thresholds
BW = (I(:,:,1) >= channel1Min ) & (I(:,:,1) <= channel1Max) & ...
    (I(:,:,2) >= channel2Min ) & (I(:,:,2) <= channel2Max) & ...
    (I(:,:,3) >= channel3Min ) & (I(:,:,3) <= channel3Max);

goalX = 4;
goalY = 4;
s = size(BW);
scale = 30;
sizeOfMap = floor(s/scale);
map = zeros(sizeOfMap(1),sizeOfMap(2));
corners =[];
map(1,:) = 1;
map(:,1) = 1;
map(sizeOfMap(1),:) = 1;
map(:,sizeOfMap(2)) = 1;
for i= 1:sizeOfMap(1)-1
    for j = 1:sizeOfMap(2)-1
        for k = 0:scale-1
            for l = 0:scale-1
                if BW(i*scale+k, j*scale+l,1) > 0
                    map(i,j) = 1; %%% points the obstacles.
                    img(i*scale:i*scale+scale,j*scale:j*scale+scale,1) = 0;  % Change the red value for the first pixel
                    img(i*scale:i*scale+scale,j*scale:j*scale+scale,2) = 0;    % Change the green value for the first pixel
                    img(i*scale:i*scale+scale,j*scale:j*scale+scale,3) = 255;    % Change the blue value for the first pixel
                end
            end
        end
    end
end

for i = 2:sizeOfMap(1)-1
    for j = 2:sizeOfMap(2)-1
        if map(i,j) ~=1 
            if (map(i-1,j) == 1 && map(i,j-1) ==1) || (map(i+1,j) == 1 && map(i,j+1) ==1) ...
                    || (map(i+1,j) == 1 && map(i,j-1) ==1) ||(map(i-1,j) == 1 && map(i,j+1) ==1)
                corners = [corners; j i];           
            end
        end
    end
end

imwrite(img,'Obstacle.jpeg');
[probability, movesX, movesY] = MDPgridworldExampleBADWALLS(map,goalX,goalY);
save('Map1', 'movesX', 'movesY','corners');

figure
imshow(img)
[X,Y] = meshgrid(1:size(map,2),1:size(map,1));
 hold on; hq=quiver(X*scale,Y*scale,movesY,movesX,0.5,'color',[0,0,0]); hold off
set(hq,'linewidth',2);
hold on
for i= 1: size(corners)
    plot(corners(i,1)*scale , corners(i,2)*scale,'*','Markersize',16,'color','red', 'linewidth',3);
end
if webcamShot
clear('cam'); % (*turns off the camera*)
end

% % img = imread('testImage2.png');  % Load a jpeg image
% test = 550;
% test2 = 400;
% hold on
% img(test,test2,:)
% img(test:test+10,test2:test2+10,1)= 255;
% img(test:test+10,test2:test2+10,2)= 0;
% img(test:test+10,test2:test2+10,3)= 0;
% % imshow(img)
