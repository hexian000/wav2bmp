#include <iostream>

#include <boost/program_options.hpp>

#include "option_error.hpp"
#include "wav2bmp.hpp"

namespace po = boost::program_options;

int main(int ac, char **av) {
	using namespace std;
	cerr << "wav2bmp 2.0\n"
		 << "  by: hexian000 Copyright(c) 2019-2020\n"
		 << endl;

	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()
			("help,h", "show this help message")
			("input,i", po::value<string>(), "set input file")
			("output,o", po::value<string>(), "set output file")
			("size,n", po::value<unsigned>(), "set FFT size")
			("resolution,r", po::value<double>(), "set horizontal resolution (default=16.0)")
			("brightness,b", po::value<double>(), "set brightness factor (default=1e-3)")
			("frequency,f", po::value<double>(), "set cut down frequency (default=440*32)")
			("logarithm,l", po::value<double>(), "logarithmic frequency axis vertical resolution (default=1.0)")
			("startfreq", po::value<double>(), "set start frequency (default=440/32)");
	options op;

	try {
		po::variables_map vm;
		po::store(po::parse_command_line(ac, av, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			cerr << desc << endl;
			return EXIT_SUCCESS;
		}

		if (vm.count("input")) {
			op.input = vm["input"].as<string>();
		} else {
			throw option_error("required option is missing: input");
		}

		if (vm.count("output")) {
			op.output = vm["output"].as<string>();
		} else {
			throw option_error("required option is missing: output");
		}

		if (vm.count("size")) {
			op.size = vm["size"].as<unsigned>();
		}

		if (vm.count("resolution")) {
			op.resolution = vm["resolution"].as<double>();
		}

		if (vm.count("brightness")) {
			op.brightness = vm["brightness"].as<double>();
		}

		if (vm.count("frequency")) {
			op.cut_down_frequency = vm["frequency"].as<double>();
		}

		if (vm.count("logarithm")) {
			op.logarithmic = vm["logarithm"].as<double>();
		}

		if (vm.count("startfreq")) {
			op.start_frequency = vm["startfreq"].as<double>();
		}

		op.check();

		wav2bmp o(op);
		o.run();
	} catch (const option_error &ex) {
		cerr << "argument error: " << ex.what() << endl;
		cerr << desc << endl;
		return EXIT_FAILURE;
	} catch (const exception &ex) {
		cerr << "error: " << ex.what() << endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
