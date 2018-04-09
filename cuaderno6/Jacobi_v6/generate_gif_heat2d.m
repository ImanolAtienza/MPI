function generate_gif_heat2d

% Number of frames
% numFrames=16;
numFrames=22;
% Time step between 2 frames
step = 0.1;
animated(1,1,1,numFrames) = 0;

% Main loop
figure(1)
i = 0
a = strcat('F:\cleo\SCP\Cleo\FORTRAN\VERSIONES\Jacobi_v6\Salida\outputPar',num2str(i,'%3.3d'),'.txt')
y=load(a);
for i=1:numFrames
a = strcat('F:\cleo\SCP\Cleo\FORTRAN\VERSIONES\Jacobi_v6\Salida\outputPar',num2str(i,'%3.3d'),'.txt')
x=load(a);
for j = 1:size(x,1)
    for k = 1:size(x,2)
      x(j,k) = x(j,k) + y(j,k);
      if x(j,k)>80
          x(j,k)=80;
      end
    end
end
          

surfc(x);
%alpha(1)
% %%%%
% lightangle(90,90)
% h.FaceLighting = 'gouraud';
% h.AmbientStrength = 0.3;
% h.DiffuseStrength = 0.8;
% h.SpecularStrength = 0.9;
% h.SpecularExponent = 25;
% h.BackFaceLighting = 'unlit';
% %%%%

shading interp;
view([0,0,1]);
colormap(jet)
hc=colorbar;
set(hc,'position',[0.932 0.3 0.02 0.6]);

caxis([53 80]);
xlabel('y domain');
ylabel('x domain');
zlabel('temperature');
 
 pause(step);    
frame = getframe(figure(1));    
if i == 1
  [animated, cmap] = rgb2ind(frame.cdata, 256, 'nodither');
else
  animated(:,:,1,i) = rgb2ind(frame.cdata, cmap, 'nodither');
end  
end
% Write final animated gif
imwrite(animated,cmap,'F:\cleo\SCP\Cleo\FORTRAN\VERSIONES\Jacobi_v6\Salida\Heat_2D.gif','DelayTime',step,'LoopCount',inf); 

end
