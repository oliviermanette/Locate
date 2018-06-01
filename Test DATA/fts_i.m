%Affiche les valeurs brutes sur un seul graphique
debut = 305000;
fin = 320000;
seuil = 12000;
subplot(2,1,1)
plot(D_L(debut:fin)>seuil)
subplot(2,1,2)
plot(D_R(debut:fin)>seuil)