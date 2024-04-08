%% Dados de linhas
% Os valores de indutância e resistência de cada LT foram retirados do
% documento do CIGRE (Strunz_2014), as LTs para a MR modelo Europeia são
% apresentadas para um sistema de 50 Hz e desbalanceada, os valores
% presentes neste script são para um sistema de 60 Hz e com somente a
% sequência positiva

%% Dados de impedância série para as LT

oh1 = (0.4917 + 0.0009i)/1000;  % Impedância para o cabo overhead, Alumínio (2.53 cm)
oh2 = (1.3207 + 0.0010i)/1000;  % Impedância para o cabo overhead, Alumínio (1.73 cm)
oh3 = (2.0167 + 0.0011i)/1000;  % Impedância para o cabo overhead, Alumínio (0.74 cm)
ug1 = (0.1620 + 0.0003i)/1000;  % Impedância para o cabo underground, Alumínio (2.53 cm)
ug2 = (0.2647 + 0.0003i)/1000;  % Impedância para o cabo underground, Alumínio (2.06 cm)    
ug3 = (0.8220 + 0.0003i)/1000;  % Impedância para o cabo underground, Alumínio (1.34 cm)

%% Valores atribuidos para cada segmento da MR:

C_1_2 = 35;                     % Comprimento do segmento entre as barras 1 - 2
R_1_2 = real(ug1)*C_1_2;        % Resistência do segmento
L_1_2 = imag(ug1)*C_1_2;        % Indutância do segmento
% X(1) = ug1*C_1_2;

C_2_3 = 35;                     % Comprimento do segmento entre as barras 2 - 3
R_2_3 = real(ug1)*C_2_3;        % Resistência do segmento
L_2_3 = imag(ug1)*C_2_3;        % Indutância do segmento
% X(2) = ug1*C_2_3;

C_3_4 = 35;                     % Comprimento do segmento entre as barras 3 - 4
R_3_4 = real(ug1)*C_3_4;        % Resistência do segmento
L_3_4 = imag(ug1)*C_3_4;        % Indutância do segmento
% X(3) = (ug1)*C_3_4;

C_4_5 = 35;                     % Comprimento do segmento entre as barras 4 - 5
R_4_5 = real(ug1)*C_4_5;        % Resistência do segmento
L_4_5 = imag(ug1)*C_4_5;        % Indutância do segmento
% X(4)=(ug1)*C_4_5;

C_5_6 = 35;                     % Comprimento do segmento entre as barras 5 - 6
R_5_6 = real(ug1)*C_5_6;        % Resistência do segmento
L_5_6 = imag(ug1)*C_5_6;        % Indutância do segmento
% X(5) = (ug1)*C_5_6;

C_6_7 = 35;                     % Comprimento do segmento entre as barras 6 - 7
R_6_7 = real(ug1)*C_6_7;        % Resistência do segmento
L_6_7 = imag(ug1)*C_6_7;        % Indutância do segmento
% X(6) = (ug1)*C_6_7;

C_7_8 = 35;                     % Comprimento do segmento entre as barras 7 - 8
R_7_8 = real(ug1)*C_7_8;        % Resistência do segmento
L_7_8 = imag(ug1)*C_7_8;        % Indutância do segmento
% X(7) = (ug1)*C_7_8;

C_8_9 = 35;                     % Comprimento do segmento entre as barras 8 - 9
R_8_9 = real(ug1)*C_8_9;        % Resistência do segmento
L_8_9 = imag(ug1)*C_8_9;        % Indutância do segmento
% X(8) = (ug1)*C_8_9;

C_9_10 = 35;                    % Comprimento do segmento entre as barras 9 - 10
R_9_10 = real(ug1)*C_9_10;      % Resistência do segmento
L_9_10 = imag(ug1)*C_9_10;      % Indutância do segmento
% X(9) = (ug1)*C_9_10;

C_3_11 = 30;                    % Comprimento do segmento entre as barras 3 - 11
R_3_11 = real(ug3)*C_3_11;      % Resistência do segmento
L_3_11 = imag(ug3)*C_3_11;      % Indutância do segmento
% X(10) = (ug3)*C_3_11;

C_4_12 = 35;                    % Comprimento do segmento entre as barras 4 - 12
R_4_12 = real(ug3)*C_4_12;      % Resistência do segmento
L_4_12 = imag(ug3)*C_4_12;      % Indutância do segmento
% X(11) = (ug3)*C_4_12;

C_12_13 = 35;                   % Comprimento do segmento entre as barras 12 - 13
R_12_13 = real(ug3)*C_12_13;    % Resistência do segmento
L_12_13 = imag(ug3)*C_12_13;    % Indutância do segmento
% X(12) = (ug3)*C_12_13;

C_13_14 = 35;                   % Comprimento do segmento entre as barras 13 - 14
R_13_14 = real(ug3)*C_13_14;    % Resistência do segmento
L_13_14 = imag(ug3)*C_13_14;    % Indutância do segmento
% X(13) = (ug3)*C_13_14;

C_14_15 = 30;                   % Comprimento do segmento entre as barras 14 - 15
R_14_15 = real(ug3)*C_14_15;    % Resistência do segmento
L_14_15 = imag(ug3)*C_14_15;    % Indutância do segmento
% X(14) = (ug3)*C_14_15;

C_6_16 = 30;                    % Comprimento do segmento entre as barras 6 - 16
R_6_16 = real(ug3)*C_6_16;      % Resistência do segmento
L_6_16 = imag(ug3)*C_6_16;      % Indutância do segmento
% X(15) = (ug3)*C_6_16;

C_9_17 = 30;                    % Comprimento do segmento entre as barras 9 - 17
R_9_17 = real(ug3)*C_9_17;      % Resistência do segmento
L_9_17 = imag(ug3)*C_9_17;      % Indutância do segmento
% X(16) = (ug3)*C_9_17;

C_10_18 = 30;                   % Comprimento do segmento entre as barras 10 - 18
R_10_18 = real(ug3)*C_10_18;    % Resistência do segmento
L_10_18 = imag(ug3)*C_10_18;    % Indutância do segmento
% X(17) = (ug3)*C_10_18;