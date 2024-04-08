%% Dados de refer�ncia para as cargas
% Esse script tem como objetivo atribuir os valores das cargas pelo hor�rio
% e permitir que o usu�srio altere/inclua incrementos de pot�ncia ativa e
% reativa nas cargas atrav�s das vari�veis:
% * P_var_loadRXX - incremento de pot�ncia ativa;
% * Q_var_loadRXX - incremento de pot�ncia reativa;
% * TLRXX - tempo de conex�o da carga;
% * TDRXX - tempo de desconex�o da carga;
% Obs.: Se TLRXX < TDRXX ent�o a carga � conectada no tempo TLRXX e
% desconectada no tempo TDRXX
% Obs.: Se TLRXX > TDRXX ent�o a carga � desconectada no tempo TLRXX e
% conectada no tempo TDRXX

%% Valores das cargas
if exist('Horario','var') == 0
    Horario = 1;
end
%%%%%%%   1  2    3  4  5    6  7  8    9 10 11 12 13 14 15 16 17 18 19  20 21 22 23   24 25 EXP;  
CdC = [27.5 25 22.5 20 15 22.5 30 40 42.5 45 50 50 55 60 60 55 50 65 85 100 90 75 57.5 30 5]/100;

%% Atribui��o das vari�veis e incremento de carga
% Dados Carga R1 - Externa � MR
PLoadR1 = 190e3*CdC(Horario);           % Pot�ncia ativa Carga R1 [W]
QLoadR1 = 62.45e3*CdC(Horario);         % Pot�ncia reativa Carga R1 [var]
TLR1 = 60;                              % Tempo de conex�o R1 [segundos]
TDR1 = 80;                              % Tempo de desconex�o R1 [segundos]
P_var_loadR1 = 10e3;                    % Pot�ncia ativa Carga R1 [W]
Q_var_loadR1 = 5e3;                     % Pot�ncia reativa Carga R1 [var]
    
% Dados Carga R11
% Carga R11 de classe 1
% Dados Carga R11
PLoadR11 = 14.25e3*CdC(Horario);        % Pot�ncia ativa Carga R11 [W]
QLoadR11 = 4.68e3*CdC(Horario);         % Pot�ncia reativa Carga R11 [var]
TLR11 = 60;                             % Tempo de conex�o R11 [segundos]
TDR11 = 80;                             % Tempo de desconex�o R11 [segundos]
P_var_loadR11 = 6e3;                    % Pot�ncia ativa Carga R11 [W]
Q_var_loadR11 = 5e3;                    % Pot�ncia reativa Carga R11 [var]
% Dados Carga R15
PLoadR15 = 49.40e3*CdC(Horario);        % Pot�ncia ativa Carga R15 [W]
QLoadR15 = 16.24e3*CdC(Horario);        % Pot�ncia reativa Carga R15 [var]
TLR15 = 3.5;                            % Tempo de conex�o R15 [segundos]
TDR15 = 80;                             % Tempo de desconex�o R15 [segundos]
P_var_loadR15 = 9e3;                    % Pot�ncia ativa Carga R15 [W]
Q_var_loadR15 = 5e3;                    % Pot�ncia reativa Carga R15 [var]
% Dados Carga R16
PLoadR16 = 52.25e3*CdC(Horario);        % Pot�ncia ativa Carga R16 [W]
QLoadR16 = 17.17e3*CdC(Horario);        % Pot�ncia reativa Carga R16 [var]
TLR16 = 6;                              % Tempo de conex�o R16 [segundos]
TDR16 = 80;                             % Tempo de desconex�o R16 [segundos]
P_var_loadR16 = 20e3;                   % Pot�ncia ativa Carga R16 [W]
Q_var_loadR16 = 13e3;                   % Pot�ncia reativa Carga R16 [var]
% Dados Carga R17
PLoadR17 = 33.25e3*CdC(Horario);        % Pot�ncia ativa Carga R17 [W]
QLoadR17 = 10.93e3*CdC(Horario);        % Pot�ncia reativa Carga R17 [var]
TLR17 = 60;                             % Tempo de conex�o R17 [segundos]
TDR17 = 80;                             % Tempo de desconex�o R17 [segundos]
P_var_loadR17 = 6e3;                    % Pot�ncia ativa Carga R17 [W]
Q_var_loadR17 = 5e3;                    % Pot�ncia reativa Carga R17 [var]
% Dados Carga R18
PLoadR18 = 44.65e3*CdC(Horario);        % Pot�ncia ativa Carga R18 [W]
QLoadR18 = 14.68e3*CdC(Horario);        % Pot�ncia reativa Carga R18 [var]
TLR18 = 60;                             % Tempo de conex�o R18 [segundos]
TDR18 = 80;                             % Tempo de desconex�o R18 [segundos]
P_var_loadR18 = 6e3;                    % Pot�ncia ativa Carga R18 [W]
Q_var_loadR18 = 5e3;                    % Pot�ncia reativa Carga R18 [var]
CargaTotal = (193.80 + 1i*63.7000)*1E3*CdC(Horario);