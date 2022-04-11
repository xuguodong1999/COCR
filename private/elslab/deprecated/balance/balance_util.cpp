#include "balance_util.hpp"
#include "std_util.hpp"
#include "fraction.hpp"
using namespace std;

int Get_proton_num(string &ele) {
    if (ele == "H")return 1;
    else if (ele == "He")return 2;
    else if (ele == "Li")return 3;
    else if (ele == "Be")return 4;
    else if (ele == "B")return 5;
    else if (ele == "C")return 6;
    else if (ele == "N")return 7;
    else if (ele == "O")return 8;
    else if (ele == "F")return 9;
    else if (ele == "Ne")return 10;
    else if (ele == "Na")return 11;
    else if (ele == "Mg")return 12;
    else if (ele == "Al")return 13;
    else if (ele == "Si")return 14;
    else if (ele == "P")return 15;
    else if (ele == "S")return 16;
    else if (ele == "Cl")return 17;
    else if (ele == "Ar")return 18;
    else if (ele == "K")return 19;
    else if (ele == "Ca")return 20;
    else if (ele == "Sc")return 21;
    else if (ele == "Ti")return 22;
    else if (ele == "V")return 23;
    else if (ele == "Cr")return 24;
    else if (ele == "Mn")return 25;
    else if (ele == "Fe")return 26;
    else if (ele == "Co")return 27;
    else if (ele == "Ni")return 28;
    else if (ele == "Cu")return 29;
    else if (ele == "Zn")return 30;
    else if (ele == "Ga")return 31;
    else if (ele == "Ge")return 32;
    else if (ele == "As")return 33;
    else if (ele == "Se")return 34;
    else if (ele == "Br")return 35;
    else if (ele == "Kr")return 36;
    else if (ele == "Rb")return 37;
    else if (ele == "Sr")return 38;
    else if (ele == "Y")return 39;
    else if (ele == "Zr")return 40;
    else if (ele == "Nb")return 41;
    else if (ele == "Mo")return 42;
    else if (ele == "Tc")return 43;
    else if (ele == "Ru")return 44;
    else if (ele == "Rh")return 45;
    else if (ele == "Pd")return 46;
    else if (ele == "Ag")return 47;
    else if (ele == "Cd")return 48;
    else if (ele == "In")return 49;
    else if (ele == "Sn")return 50;
    else if (ele == "Sb")return 51;
    else if (ele == "Te")return 52;
    else if (ele == "I")return 53;
    else if (ele == "Xe")return 54;
    else if (ele == "Cs")return 55;
    else if (ele == "Ba")return 56;
    else if (ele == "La")return 57;
    else if (ele == "Ce")return 58;
    else if (ele == "Pr")return 59;
    else if (ele == "Nd")return 60;
    else if (ele == "Pm")return 61;
    else if (ele == "Sm")return 62;
    else if (ele == "Eu")return 63;
    else if (ele == "Gd")return 64;
    else if (ele == "Tb")return 65;
    else if (ele == "Dy")return 66;
    else if (ele == "Ho")return 67;
    else if (ele == "Er")return 68;
    else if (ele == "Tm")return 69;
    else if (ele == "Yb")return 70;
    else if (ele == "Lu")return 71;
    else if (ele == "Hf")return 72;
    else if (ele == "Ta")return 73;
    else if (ele == "W")return 74;
    else if (ele == "Re")return 75;
    else if (ele == "Os")return 76;
    else if (ele == "Ir")return 77;
    else if (ele == "Pt")return 78;
    else if (ele == "Au")return 79;
    else if (ele == "Hg")return 80;
    else if (ele == "Tl")return 81;
    else if (ele == "Pb")return 82;
    else if (ele == "Bi")return 83;
    else if (ele == "Po")return 84;
    else if (ele == "At")return 85;
    else if (ele == "Rn")return 86;
    else if (ele == "Fr")return 87;
    else if (ele == "Ra")return 88;
    else if (ele == "Ac")return 89;
    else if (ele == "Th")return 90;
    else if (ele == "Pa")return 91;
    else if (ele == "U")return 92;
    else if (ele == "Np")return 93;
    else if (ele == "Pu")return 94;
    else if (ele == "Am")return 95;
    else if (ele == "Cm")return 96;
    else if (ele == "Bk")return 97;
    else if (ele == "Cf")return 98;
    else if (ele == "Es")return 99;
    else if (ele == "Fm")return 100;
    else if (ele == "Md")return 101;
    else if (ele == "No")return 102;
    else if (ele == "Lr")return 103;
    else if (ele == "Rf")return 104;
    else if (ele == "Db")return 105;
    else if (ele == "Sg")return 106;
    else if (ele == "Bh")return 107;
    else if (ele == "Hs")return 108;
    else if (ele == "Mt")return 109;
    else if (ele == "Ds")return 110;
    else if (ele == "Rg")return 111;
    else if (ele == "Cn")return 112;
    else return -1;
}

double Get_relative_atomic_mass(string &ele) {
    double relative_atomic_mass = 0;
    switch (Get_proton_num(ele)) {
        case 0:
            relative_atomic_mass = 0;
            break;
        case 1:
            relative_atomic_mass = 1.007947;
            break;
        case 2:
            relative_atomic_mass = 4.0026022;
            break;
        case 3:
            relative_atomic_mass = 6.9412;
            break;
        case 4:
            relative_atomic_mass = 9.0121823;
            break;
        case 5:
            relative_atomic_mass = 10.8117;
            break;
        case 6:
            relative_atomic_mass = 12.01078;
            break;
        case 7:
            relative_atomic_mass = 14.00672;
            break;
        case 8:
            relative_atomic_mass = 15.99943;
            break;
        case 9:
            relative_atomic_mass = 18.99840325;
            break;
        case 10:
            relative_atomic_mass = 20.17976;
            break;
        case 11:
            relative_atomic_mass = 22.989769282;
            break;
        case 12:
            relative_atomic_mass = 24.30506;
            break;
        case 13:
            relative_atomic_mass = 26.98153868;
            break;
        case 14:
            relative_atomic_mass = 28.08553;
            break;
        case 15:
            relative_atomic_mass = 30.9737622;
            break;
        case 16:
            relative_atomic_mass = 32.0655;
            break;
        case 17:
            relative_atomic_mass = 35.4532;
            break;
        case 18:
            relative_atomic_mass = 39.9481;
            break;
        case 19:
            relative_atomic_mass = 39.09831;
            break;
        case 20:
            relative_atomic_mass = 40.0784;
            break;
        case 21:
            relative_atomic_mass = 44.9559126;
            break;
        case 22:
            relative_atomic_mass = 47.8671;
            break;
        case 23:
            relative_atomic_mass = 50.94151;
            break;
        case 24:
            relative_atomic_mass = 51.99616;
            break;
        case 25:
            relative_atomic_mass = 54.9380455;
            break;
        case 26:
            relative_atomic_mass = 55.8452;
            break;
        case 27:
            relative_atomic_mass = 58.9331955;
            break;
        case 28:
            relative_atomic_mass = 58.69342;
            break;
        case 29:
            relative_atomic_mass = 63.5463;
            break;
        case 30:
            relative_atomic_mass = 65.394;
            break;
        case 31:
            relative_atomic_mass = 69.7231;
            break;
        case 32:
            relative_atomic_mass = 72.641;
            break;
        case 33:
            relative_atomic_mass = 74.921602;
            break;
        case 34:
            relative_atomic_mass = 78.963;
            break;
        case 35:
            relative_atomic_mass = 79.9041;
            break;
        case 36:
            relative_atomic_mass = 83.7982;
            break;
        case 37:
            relative_atomic_mass = 85.46783;
            break;
        case 38:
            relative_atomic_mass = 87.621;
            break;
        case 39:
            relative_atomic_mass = 88.905852;
            break;
        case 40:
            relative_atomic_mass = 91.2242;
            break;
        case 41:
            relative_atomic_mass = 92.906382;
            break;
        case 42:
            relative_atomic_mass = 95.942;
            break;
        case 43:
            relative_atomic_mass = 97.9072;
            break;
        case 44:
            relative_atomic_mass = 101.072;
            break;
        case 45:
            relative_atomic_mass = 102.905502;
            break;
        case 46:
            relative_atomic_mass = 106.421;
            break;
        case 47:
            relative_atomic_mass = 107.86822;
            break;
        case 48:
            relative_atomic_mass = 112.4118;
            break;
        case 49:
            relative_atomic_mass = 114.8183;
            break;
        case 50:
            relative_atomic_mass = 118.7107;
            break;
        case 51:
            relative_atomic_mass = 121.7601;
            break;
        case 52:
            relative_atomic_mass = 127.603;
            break;
        case 53:
            relative_atomic_mass = 126.904473;
            break;
        case 54:
            relative_atomic_mass = 131.2936;
            break;
        case 55:
            relative_atomic_mass = 132.90545192;
            break;
        case 56:
            relative_atomic_mass = 137.3277;
            break;
        case 57:
            relative_atomic_mass = 137.3277;
            break;
        case 58:
            relative_atomic_mass = 140.1161;
            break;
        case 59:
            relative_atomic_mass = 140.907652;
            break;
        case 60:
            relative_atomic_mass = 144.2423;
            break;
        case 61:
            relative_atomic_mass = 145;
            break;
        case 62:
            relative_atomic_mass = 150.362;
            break;
        case 63:
            relative_atomic_mass = 151.9641;
            break;
        case 64:
            relative_atomic_mass = 157.253;
            break;
        case 65:
            relative_atomic_mass = 158.925352;
            break;
        case 66:
            relative_atomic_mass = 162.5001;
            break;
        case 67:
            relative_atomic_mass = 164.930322;
            break;
        case 68:
            relative_atomic_mass = 167.2593;
            break;
        case 69:
            relative_atomic_mass = 168.934212;
            break;
        case 70:
            relative_atomic_mass = 173.043;
            break;
        case 71:
            relative_atomic_mass = 174.9671;
            break;
        case 72:
            relative_atomic_mass = 178.492;
            break;
        case 73:
            relative_atomic_mass = 180.947882;
            break;
        case 74:
            relative_atomic_mass = 183.841;
            break;
        case 75:
            relative_atomic_mass = 186.2071;
            break;
        case 76:
            relative_atomic_mass = 190.233;
            break;
        case 77:
            relative_atomic_mass = 192.2173;
            break;
        case 78:
            relative_atomic_mass = 195.0849;
            break;
        case 79:
            relative_atomic_mass = 196.9665694;
            break;
        case 80:
            relative_atomic_mass = 200.592;
            break;
        case 81:
            relative_atomic_mass = 204.38332;
            break;
        case 82:
            relative_atomic_mass = 207.21;
            break;
        case 83:
            relative_atomic_mass = 208.980401;
            break;
        case 84:
            relative_atomic_mass = 208.9824;
            break;
        case 85:
            relative_atomic_mass = 209.9871;
            break;
        case 86:
            relative_atomic_mass = 222.0176;
            break;
        case 87:
            relative_atomic_mass = 223;
            break;
        case 88:
            relative_atomic_mass = 226;
            break;
        case 89:
            relative_atomic_mass = 227;
            break;
        case 90:
            relative_atomic_mass = 232.038062;
            break;
        case 91:
            relative_atomic_mass = 231.035882;
            break;
        case 92:
            relative_atomic_mass = 238.028913;
            break;
        case 93:
            relative_atomic_mass = 238.8486;
            break;
        case 94:
            relative_atomic_mass = 242.8798;
            break;
        case 95:
            relative_atomic_mass = 244.8594;
            break;
        case 96:
            relative_atomic_mass = 246.911;
            break;
        case 97:
            relative_atomic_mass = 248.9266;
            break;
        case 98:
            relative_atomic_mass = 252.9578;
            break;
        case 99:
            relative_atomic_mass = 253.9656;
            break;
        case 100:
            relative_atomic_mass = 259.0046;
            break;
        case 101:
            relative_atomic_mass = 260.0124;
            break;
        case 102:
            relative_atomic_mass = 261.0202;
            break;
        case 103:
            relative_atomic_mass = 264.0436;
            break;
        case 104:
            relative_atomic_mass = 269.0826;
            break;
        case 105:
            relative_atomic_mass = 270.0904;
            break;
        case 106:
            relative_atomic_mass = 273.1138;
            break;
        case 107:
            relative_atomic_mass = 274.1216;
            break;
        case 108:
            relative_atomic_mass = 272.106;
            break;
        case 109:
            relative_atomic_mass = 278.1528;
            break;
        case 110:
            relative_atomic_mass = 283.1918;
            break;
        case 111:
            relative_atomic_mass = 282.184;
            break;
        case 112:
            relative_atomic_mass = 287.223;
            break;
        default:
            relative_atomic_mass = -1;
            break;
    }
    return relative_atomic_mass;
}

double Get_relative_atomic_mass(int proton_num) {
    double relative_atomic_mass = 0;
    switch (proton_num) {
        case 0:
            relative_atomic_mass = 0;
            break;
        case 1:
            relative_atomic_mass = 1.007947;
            break;
        case 2:
            relative_atomic_mass = 4.0026022;
            break;
        case 3:
            relative_atomic_mass = 6.9412;
            break;
        case 4:
            relative_atomic_mass = 9.0121823;
            break;
        case 5:
            relative_atomic_mass = 10.8117;
            break;
        case 6:
            relative_atomic_mass = 12.01078;
            break;
        case 7:
            relative_atomic_mass = 14.00672;
            break;
        case 8:
            relative_atomic_mass = 15.99943;
            break;
        case 9:
            relative_atomic_mass = 18.99840325;
            break;
        case 10:
            relative_atomic_mass = 20.17976;
            break;
        case 11:
            relative_atomic_mass = 22.989769282;
            break;
        case 12:
            relative_atomic_mass = 24.30506;
            break;
        case 13:
            relative_atomic_mass = 26.98153868;
            break;
        case 14:
            relative_atomic_mass = 28.08553;
            break;
        case 15:
            relative_atomic_mass = 30.9737622;
            break;
        case 16:
            relative_atomic_mass = 32.0655;
            break;
        case 17:
            relative_atomic_mass = 35.4532;
            break;
        case 18:
            relative_atomic_mass = 39.9481;
            break;
        case 19:
            relative_atomic_mass = 39.09831;
            break;
        case 20:
            relative_atomic_mass = 40.0784;
            break;
        case 21:
            relative_atomic_mass = 44.9559126;
            break;
        case 22:
            relative_atomic_mass = 47.8671;
            break;
        case 23:
            relative_atomic_mass = 50.94151;
            break;
        case 24:
            relative_atomic_mass = 51.99616;
            break;
        case 25:
            relative_atomic_mass = 54.9380455;
            break;
        case 26:
            relative_atomic_mass = 55.8452;
            break;
        case 27:
            relative_atomic_mass = 58.9331955;
            break;
        case 28:
            relative_atomic_mass = 58.69342;
            break;
        case 29:
            relative_atomic_mass = 63.5463;
            break;
        case 30:
            relative_atomic_mass = 65.394;
            break;
        case 31:
            relative_atomic_mass = 69.7231;
            break;
        case 32:
            relative_atomic_mass = 72.641;
            break;
        case 33:
            relative_atomic_mass = 74.921602;
            break;
        case 34:
            relative_atomic_mass = 78.963;
            break;
        case 35:
            relative_atomic_mass = 79.9041;
            break;
        case 36:
            relative_atomic_mass = 83.7982;
            break;
        case 37:
            relative_atomic_mass = 85.46783;
            break;
        case 38:
            relative_atomic_mass = 87.621;
            break;
        case 39:
            relative_atomic_mass = 88.905852;
            break;
        case 40:
            relative_atomic_mass = 91.2242;
            break;
        case 41:
            relative_atomic_mass = 92.906382;
            break;
        case 42:
            relative_atomic_mass = 95.942;
            break;
        case 43:
            relative_atomic_mass = 97.9072;
            break;
        case 44:
            relative_atomic_mass = 101.072;
            break;
        case 45:
            relative_atomic_mass = 102.905502;
            break;
        case 46:
            relative_atomic_mass = 106.421;
            break;
        case 47:
            relative_atomic_mass = 107.86822;
            break;
        case 48:
            relative_atomic_mass = 112.4118;
            break;
        case 49:
            relative_atomic_mass = 114.8183;
            break;
        case 50:
            relative_atomic_mass = 118.7107;
            break;
        case 51:
            relative_atomic_mass = 121.7601;
            break;
        case 52:
            relative_atomic_mass = 127.603;
            break;
        case 53:
            relative_atomic_mass = 126.904473;
            break;
        case 54:
            relative_atomic_mass = 131.2936;
            break;
        case 55:
            relative_atomic_mass = 132.90545192;
            break;
        case 56:
            relative_atomic_mass = 137.3277;
            break;
        case 57:
            relative_atomic_mass = 137.3277;
            break;
        case 58:
            relative_atomic_mass = 140.1161;
            break;
        case 59:
            relative_atomic_mass = 140.907652;
            break;
        case 60:
            relative_atomic_mass = 144.2423;
            break;
        case 61:
            relative_atomic_mass = 145;
            break;
        case 62:
            relative_atomic_mass = 150.362;
            break;
        case 63:
            relative_atomic_mass = 151.9641;
            break;
        case 64:
            relative_atomic_mass = 157.253;
            break;
        case 65:
            relative_atomic_mass = 158.925352;
            break;
        case 66:
            relative_atomic_mass = 162.5001;
            break;
        case 67:
            relative_atomic_mass = 164.930322;
            break;
        case 68:
            relative_atomic_mass = 167.2593;
            break;
        case 69:
            relative_atomic_mass = 168.934212;
            break;
        case 70:
            relative_atomic_mass = 173.043;
            break;
        case 71:
            relative_atomic_mass = 174.9671;
            break;
        case 72:
            relative_atomic_mass = 178.492;
            break;
        case 73:
            relative_atomic_mass = 180.947882;
            break;
        case 74:
            relative_atomic_mass = 183.841;
            break;
        case 75:
            relative_atomic_mass = 186.2071;
            break;
        case 76:
            relative_atomic_mass = 190.233;
            break;
        case 77:
            relative_atomic_mass = 192.2173;
            break;
        case 78:
            relative_atomic_mass = 195.0849;
            break;
        case 79:
            relative_atomic_mass = 196.9665694;
            break;
        case 80:
            relative_atomic_mass = 200.592;
            break;
        case 81:
            relative_atomic_mass = 204.38332;
            break;
        case 82:
            relative_atomic_mass = 207.21;
            break;
        case 83:
            relative_atomic_mass = 208.980401;
            break;
        case 84:
            relative_atomic_mass = 208.9824;
            break;
        case 85:
            relative_atomic_mass = 209.9871;
            break;
        case 86:
            relative_atomic_mass = 222.0176;
            break;
        case 87:
            relative_atomic_mass = 223;
            break;
        case 88:
            relative_atomic_mass = 226;
            break;
        case 89:
            relative_atomic_mass = 227;
            break;
        case 90:
            relative_atomic_mass = 232.038062;
            break;
        case 91:
            relative_atomic_mass = 231.035882;
            break;
        case 92:
            relative_atomic_mass = 238.028913;
            break;
        case 93:
            relative_atomic_mass = 238.8486;
            break;
        case 94:
            relative_atomic_mass = 242.8798;
            break;
        case 95:
            relative_atomic_mass = 244.8594;
            break;
        case 96:
            relative_atomic_mass = 246.911;
            break;
        case 97:
            relative_atomic_mass = 248.9266;
            break;
        case 98:
            relative_atomic_mass = 252.9578;
            break;
        case 99:
            relative_atomic_mass = 253.9656;
            break;
        case 100:
            relative_atomic_mass = 259.0046;
            break;
        case 101:
            relative_atomic_mass = 260.0124;
            break;
        case 102:
            relative_atomic_mass = 261.0202;
            break;
        case 103:
            relative_atomic_mass = 264.0436;
            break;
        case 104:
            relative_atomic_mass = 269.0826;
            break;
        case 105:
            relative_atomic_mass = 270.0904;
            break;
        case 106:
            relative_atomic_mass = 273.1138;
            break;
        case 107:
            relative_atomic_mass = 274.1216;
            break;
        case 108:
            relative_atomic_mass = 272.106;
            break;
        case 109:
            relative_atomic_mass = 278.1528;
            break;
        case 110:
            relative_atomic_mass = 283.1918;
            break;
        case 111:
            relative_atomic_mass = 282.184;
            break;
        case 112:
            relative_atomic_mass = 287.223;
            break;
        default:
            relative_atomic_mass = 0;
            break;
    }
    return relative_atomic_mass;
}

double Get_electronegativity(int proton_num) {
    double res = 0;
    switch (proton_num) {
        case 1:
            res = 2.20;
            break;
        case 2:
            res = 3.89;
            break;
        case 3:
            res = 0.98;
            break;
        case 4:
            res = 1.57;
            break;
        case 5:
            res = 2.04;
            break;
        case 6:
            res = 2.55;
            break;
        case 7:
            res = 3.04;
            break;
        case 8:
            res = 3.44;
            break;
        case 9:
            res = 3.98;
            break;
        case 10:
            res = 3.67;
            break;
        case 11:
            res = 0.93;
            break;
        case 12:
            res = 1.31;
            break;
        case 13:
            res = 1.61;
            break;
        case 14:
            res = 1.90;
            break;
        case 15:
            res = 2.19;
            break;
        case 16:
            res = 2.58;
            break;
        case 17:
            res = 3.16;
            break;
        case 18:
            res = 3.3;
            break;
        case 19:
            res = 0.82;
            break;
        case 20:
            res = 1.00;
            break;
        case 21:
            res = 1.36;
            break;
        case 22:
            res = 1.54;
            break;
        case 23:
            res = 1.63;
            break;
        case 24:
            res = 1.66;
            break;
        case 25:
            res = 1.55;
            break;
        case 26:
            res = 1.83;
            break;
        case 27:
            res = 1.88;
            break;
        case 28:
            res = 1.91;
            break;
        case 29:
            res = 1.90;
            break;
        case 30:
            res = 1.65;
            break;
        case 31:
            res = 1.81;
            break;
        case 32:
            res = 2.01;
            break;
        case 33:
            res = 2.18;
            break;
        case 34:
            res = 2.55;
            break;
        case 35:
            res = 2.96;
            break;
        case 36:
            res = 3.00;
            break;
        case 37:
            res = 0.82;
            break;
        case 38:
            res = 0.95;
            break;
        case 39:
            res = 1.22;
            break;
        case 40:
            res = 1.33;
            break;
        case 41:
            res = 1.6;
            break;
        case 42:
            res = 2.16;
            break;
        case 43:
            res = 1.9;
            break;
        case 44:
            res = 2.2;
            break;
        case 45:
            res = 2.28;
            break;
        case 46:
            res = 2.20;
            break;
        case 47:
            res = 1.93;
            break;
        case 48:
            res = 1.69;
            break;
        case 49:
            res = 1.78;
            break;
        case 50:
            res = 1.96;
            break;
        case 51:
            res = 2.05;
            break;
        case 52:
            res = 2.1;
            break;
        case 53:
            res = 2.66;
            break;
        case 54:
            res = 2.67;
            break;
        case 55:
            res = 0.97;
            break;
        case 56:
            res = 0.89;
            break;
        case 57:
            res = 1.1;
            break;
        case 58:
            res = 1.12;
            break;
        case 59:
            res = 1.13;
            break;
        case 60:
            res = 1.14;
            break;
        case 61:
            res = 1.13;
            break;
        case 62:
            res = 1.17;
            break;
        case 63:
            res = 1.2;
            break;
        case 64:
            res = 1.2;
            break;
        case 65:
            res = 1.1;
            break;
        case 66:
            res = 1.22;
            break;
        case 67:
            res = 1.23;
            break;
        case 68:
            res = 1.24;
            break;
        case 69:
            res = 1.25;
            break;
        case 70:
            res = 1.1;
            break;
        case 71:
            res = 1.27;
            break;
        case 72:
            res = 1.3;
            break;
        case 73:
            res = 1.5;
            break;
        case 74:
            res = 2.36;
            break;
        case 75:
            res = 1.9;
            break;
        case 76:
            res = 2.2;
            break;
        case 77:
            res = 2.20;
            break;
        case 78:
            res = 2.28;
            break;
        case 79:
            res = 2.54;
            break;
        case 80:
            res = 2.00;
            break;
        case 81:
            res = 1.62;
            break;
        case 82:
            res = 2.33;
            break;
        case 83:
            res = 2.02;
            break;
        case 84:
            res = 2.0;
            break;
        case 85:
            res = 2.2;
            break;
        case 86:
            res = 2.2;
            break;
        case 87:
            res = 0.7;
            break;
        case 88:
            res = 0.9;
            break;
        case 89:
            res = 1.1;
            break;
        case 90:
            res = 1.3;
            break;
        case 91:
            res = 1.5;
            break;
        case 92:
            res = 1.38;
            break;
        case 93:
            res = 1.36;
            break;
        case 94:
            res = 1.28;
            break;
        case 95:
            res = 1.13;
            break;
        case 96:
            res = 1.28;
            break;
        case 97:
            res = 1.3;
            break;
        case 98:
            res = 1.3;
            break;
        case 99:
            res = 1.3;
            break;
        case 100:
            res = 1.3;
            break;
        case 101:
            res = 1.3;
            break;
        case 102:
            res = 1.3;
            break;
        case 103:
            res = 1.291;
            break;
        case 104:
            res = 1.3;
            break;
        case 105:
            res = 1.5;
            break;
        case 106:
            res = 2.36;
            break;
        case 107:
            res = 1.9;
            break;
        case 108:
            res = 2.2;
            break;
        case 109:
            res = 2.20;
            break;
        case 110:
            res = 2.28;
            break;
        case 111:
            res = 2.54;
            break;
        case 112:
            res = 2.00;
            break;
        default:
            res = -1;
            break;
    }
    return res;
}

string add_html(const string &_tar) {
    auto tar = replaceSubStr(replaceSubStr(replaceSubStr(replaceSubStr(replaceSubStr(
            _tar, "·", "."), "[", "("), "{", "("), "]", ")"), "}", ")");
    string res;
    int n = (int) tar.length();
    for (int i = 0; i < n; i++) {
        if (tar[i] == '.') {
            res += "·";
            if (i + 1 < n && tar[i + 1] >= '0' && tar[i + 1] <= '9') {
                i++;
                while (tar[i] >= '0' && tar[i] <= '9')
                    res += tar[i++];
                i--;
            }
        } else if (tar[i] >= '0' && tar[i] <= '9') {
            res += "<sub>";
            while (tar[i] >= '0' && tar[i] <= '9')
                res += tar[i++];
            i--;

            res += "</sub>";
        } else
            res += tar[i];
    }
    return res;
}

frac to_frac(const std::string &_src) {
//        char *ca = new char[MAXC];
//        char *cb = new char[MAXC];
    std::string ca, cb;
    int lentar = _src.length();
    int i = 0;
    int ia = 0;
    int ib = 0;
    bool is_a = 1;
    while (i < lentar) {
        if (_src[i] == '/') {
            is_a = 0;
            i++;
        } else if (is_a)
            ca[ia++] = _src[i++];
        else
            cb[ib++] = _src[i++];
    }
    frac val;
    val.set(std::stoi(ca), std::stoi(cb));
    return std::move(val);
}
