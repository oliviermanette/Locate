%max abs valeur et position
%position = 1;
%trial = 1;
GLOBAL_THRESHOLD  = 512;
numberPos = 6;
numberTrial = 7;
X = zeros(numberPos*(numberTrial+1),7);
for position=1:numberPos
    for trial=1:2:((numberTrial*2)-1)
        tmpVr= eval(['peaks',num2str(position)]);
        [~, indiceT] = max(abs(tmpVr(:,trial))>512);
        [valeur, indice] = max(abs(tmpVr(:,trial)));
        X(1+(position-1)*5+(trial-1),1) = indice - indiceT;
        X(1+(position-1)*5+(trial-1),2) = valeur;
        X(1+(position-1)*5+(trial-1),3) = mean(abs(tmpVr(:,trial)));

        [~, indiceT] = max(abs(tmpVr(:,trial+1))>512);
        [valeur, indice] = max(abs(tmpVr(:,trial+1)));
        X(1+(position-1)*5+(trial-1),4) = indice - indiceT;
        X(1+(position-1)*5+(trial-1),5) = valeur;
        X(1+(position-1)*5+(trial-1),6) = mean(abs(tmpVr(:,trial+1)));
        
        X(1+(position-1)*5+(trial-1),7) = position;
    end
end
strLesTitres=["Timing max";"Max Value";"Power"];
for lintShift=1:3
    figure(lintShift);
    hold on; 
    title(strLesTitres(lintShift));
    plot(X(:,7),X(:,lintShift),'rx');
    
    plot(X(:,7),X(:,lintShift+3),'ko')
end