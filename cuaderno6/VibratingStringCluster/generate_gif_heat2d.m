
function generate_gif_heat2d


x=load(strcat('F:\cleo\SCP\Cleo\FORTRAN\VibratingString\outputPar000.txt'));
mesh(x);
surf(x);
rotate3d on;
alpha(.5)

shading interp;
colormap(jet)
caxis([-1 1]);
xlabel('h domain');
ylabel('t domain');
zlabel('amplitud');
hc=colorbar;
set(hc,'position',[0.932 0.3 0.02 0.6]);

end