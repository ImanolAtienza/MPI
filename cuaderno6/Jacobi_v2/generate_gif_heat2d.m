function generate_gif_heat2d

% Number of frames
% numFrames=16;
numFrames=65;
% Time step between 2 frames
step = 0.01;
animated(1,1,1,numFrames) = 0;

% Main loop
figure(1)
for i=1:numFrames

x=load(strcat('F:\cleo\SCP\Cleo\FORTRAN\VERSIONES\Jacobi_v2\Salida\outputPar',num2str(i,'%3.3d'),'.txt'));
% x=load(strcat('F:\cleo\TallerMPIFortran\EMULAR_MPI_xx\E_gather_v\resultados\outputPar',num2str(i,'%3.3d'),'.txt'));
surfc(x);
shading interp;
view([0,0,1]);
colormap(jet)
hc=colorbar;
set(hc,'position',[0.932 0.3 0.02 0.6]);
% caxis([0 16]);
caxis([20 40]);
xlabel('x domain');
ylabel('y domain');
zlabel('temperature');
% Set pause 
 pause(step);    
frame = getframe(figure(1));    
if i == 1
  [animated, cmap] = rgb2ind(frame.cdata, 256, 'nodither');
else
  animated(:,:,1,i) = rgb2ind(frame.cdata, cmap, 'nodither');
end  
end
% Write final animated gif
imwrite(animated,cmap,'F:\cleo\SCP\Cleo\FORTRAN\VERSIONES\Jacobi_v2\Salida\Heat_2D.gif','DelayTime',step,'LoopCount',inf); 
% imwrite(animated,cmap,'F:\cleo\TallerMPIFortran\EMULAR_MPI_xx\E_gather_v\resultados\outputPar.gif','DelayTime',step,'LoopCount',inf); 

end
