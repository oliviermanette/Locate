%
%plot(R,'r');hold on;plot(L,'b');
position=4;
graphcolor=['kx','rx','gx','bx'];
tmpVr=eval(['R',num2str(position)]);
tmpVl=eval(['L',num2str(position)]);
[~,PositionsR]=max(abs(tmpVr(1:400,:)));
[~,PositionsL]=max(abs(tmpVl(1:400,:)));
rpos = PositionsL./PositionsR
rmean = mean(abs(tmpVr))./mean(abs(tmpVl))
figure(1);
hold on
plot(rpos,rmean,graphcolor(position));
%rmax = max(abs(tmpVr))./max(abs(tmpVl))
mean([rpos,rmean])