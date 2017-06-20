__kernel void hello(__global char* string)
{
string[0] = 'H';
string[1] = 'E';
string[2] = 'L';
string[3] = 'L';
string[4] = 'O';
string[5] = ',';
string[6] = ' ';
string[7] = 'W';
string[8] = 'o';
string[9] = 'r';
string[10] = 'l';
string[11] = 'd';
string[12] = '!';
string[13] = '\0';
}
