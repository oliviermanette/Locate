%max abs valeur et position
%position = 1;
%trial = 1;
GLOBAL_THRESHOLD  = 512;
X = zeros(33,7);
for position=1:6
    for trial=1:2:9
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