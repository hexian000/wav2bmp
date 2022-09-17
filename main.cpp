#include <iostream>

#include <boost/program_options.hpp>

#include "option_error.hpp"
#include "wav2bmp_error.hpp"
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
			("size,n", po::value<unsigned>()->default_value(4096u), "set FFT size")
			("resolution,r", po::value<double>()->default_value(16.0), "set horizontal resolution")
			("brightness,b", po::value<double>()->default_value(1e-3), "set brightness factor")
			("frequency,f", po::value<double>()->default_value(440.0 * 32.0), "set cut down frequency")
			("logarithm,l", po::value<double>()->default_value(0.0)->implicit_value(1.0),
			 "logarithmic frequency axis vertical resolution")
			("startfreq", po::value<double>()->default_value(440.0 / 32.0), "set start frequency");
	options op;

	try {
		po::variables_map vm;
		po::store(po::parse_command_line(ac, av, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			cerr << desc << endl;
			return EXIT_SUCCESS;
		}

		if (!vm.count("input")) {
			throw option_error("required option is missing: input");
		}
		if (!vm.count("output")) {
			throw option_error("required option is missing: output");
		}

		op.input = vm["input"].as<string>();
		op.output = vm["output"].as<string>();
		op.size = vm["size"].as<unsigned>();
		op.resolution = vm["resolution"].as<double>();
		op.brightness = vm["brightness"].as<double>();
		op.cut_down_frequency = vm["frequency"].as<double>();
		op.logarithmic = vm["logarithm"].as<double>();
		op.start_frequency = vm["startfreq"].as<double>();

		op.check();

		wav2bmp o(op);
		o.run();
	} catch (const option_error &ex) {
		cerr << "argument error: " << ex.what() << endl;
		cerr << desc << endl;
		return EXIT_FAILURE;
	} catch (const wav2bmp_error &ex) {
		cerr << "transform error: " << ex.what() << endl;
		return EXIT_FAILURE;
	} catch (const exception &ex) {
		cerr << "error: " << ex.what() << endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
