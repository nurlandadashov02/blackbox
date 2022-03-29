struct input{
    string path<>;
	int number1;
	int number2;
};

program ADD_PROG{
	version ADD_VERS{
		string execute(input)=1;
	}=1;
}=0x12345678;