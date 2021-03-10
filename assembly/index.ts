import * as llio from './llio';

const version: i32 = 0;

export function entry(): i32 {

	const str: string = "version " + version.toString(); // ERROR
	//const str: string = "version "; // OK

	llio.printw(str);
	return 42;
}
