  % Find maximum response
   %I = im2double(imread('BadResult.png'));
   I = im2double(imread('TestImage2.png'));
   
   % %next 2 lines are code useful for designing the template
   %  figure(1); clf;image(I); center = round(ginput(1)); display(center)%
   %  RAD = 11;
    % xTemplate=(center(1)- RAD:center(1)+ RAD);yTemplate=(center(2)- RAD:center(2)+ RAD);
     %figure(1);clf;image(I(yTemplate,xTemplate,:));title(mat2str(size(I(yTemplate,xTemplate,1))));axis equal tight
   %display(['xTemplate=(',num2str(center(1)-7),':',num2str(center(1)+7),');','yTemplate=(',num2str(center(2)-7),':',num2str(center(2)+7),');'])
   % xTemplate=(559:573);yTemplate=(212:226);
   % figure(1);clf;image(I(yTemplate,xTemplate,:));title(mat2str(size(I(yTemplate,xTemplate,1))));axis equal tight
xTemplate=(1175:1189);yTemplate=(290:304);
   
   
  % Template of Eye Lena
   T=I(yTemplate,xTemplate,:);
  % Calculate SSD and NCC between Template and Image
   [I_SSD,I_NCC]=template_matching(T,I);
  % Find maximum correspondence in I_SDD image
   [x,y]=find(I_SSD==max(I_SSD(:)));
  % Show result
   figure(2) 
   subplot(2,2,1), imshow(I); hold on; plot(y,x,'r*'); title('Result')
   subplot(2,2,2), imshow(T); title('The robot template');
   subplot(2,2,3), imshow(I_SSD); title('SSD Matching');
   subplot(2,2,4), imshow(I_NCC); title('Normalized-CC');
   
   
   BW = im2bw(I_SSD, 0.25);
   figure(3);BW = im2bw(I_NCC, 0.25);imshow(BW);
   
   