/*
 * TerminalAttributes.cpp
 *
 *  Created on: Apr 30, 2014
 *      Author: enrico
 */

#include "TerminalSettings.h"

#include <termios.h>

#include <iostream>

TerminalSettings::TerminalSettings(int file_descriptor, bool print)
{
	term_settings = termios();
	if (tcgetattr(file_descriptor, &term_settings) == 0) {
		icanon = (ICANON & term_settings.c_iflag) != 0;
		icrnl = (ICRNL & term_settings.c_iflag) != 0;
		igncr = (IGNCR & term_settings.c_iflag) != 0;

		if (print) {
			std::cout << "ICANON " << icanon << std::endl;
			std::cout << "ICRNL " << icrnl << std::endl;
			std::cout << "IGNCR " << igncr << std::endl;
		}
	} else {
		std::cerr << "Error in tcgetattr" <<std::endl;
		throw std::exception();
	}
}

TerminalSettings::~TerminalSettings() {
}

