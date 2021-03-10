#include "./wasm3/source/wasm3.h"
#include "./wasm3/source/m3_api_defs.h"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <codecvt>
#include <locale>

using namespace std;

static string w2c(const char16_t *wstr) {
	return wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(wstr);
}

static void check_m3(string msg, M3Result res) {
	/*
		We ignore functionLookupFailed here too because
		it just means the WASM payload didn't need it.
		It's not harmful to ignore.
	*/
	if (res != m3Err_none && res != m3Err_functionLookupFailed) {
		throw runtime_error(msg + ": " + res);
	}
}

m3ApiRawFunction(do_abortw) {
	m3ApiReturnType(void);
	m3ApiGetArgMem(const char16_t *, message_s);
	m3ApiGetArgMem(const char16_t *, filename_s);
	m3ApiGetArg(int32_t, line);
	m3ApiGetArg(int32_t, col);

	cout << "WASM ABORT: " << w2c(message_s) << endl;
	cout << "              at " << w2c(filename_s) << ":" << line << col << endl;

	m3ApiSuccess();
}

m3ApiRawFunction(do_printw) {
	m3ApiReturnType(void);
	m3ApiGetArgMem(const char16_t *, message_s);

	cout << "[wasm] " << w2c(message_s) << endl;

	m3ApiSuccess();
}

int pmain() {
	IM3Environment m3_env = m3_NewEnvironment();
	assert(m3_env != nullptr);

	IM3Runtime m3_rt = m3_NewRuntime(
		m3_env,
		1024 * 16,
		nullptr
	);
	assert(m3_rt != nullptr);

	ifstream ifs("./optimized.wasm", ios_base::ate | ios_base::binary);
	assert(ifs.is_open());

	auto size = ifs.tellg();
	vector<uint8_t> data(size);
	ifs.seekg(0, ios_base::beg);
	ifs.read((char *) data.data(), size);

	IM3Module m3_mod = nullptr;

	check_m3("m3_ParseModule()", m3_ParseModule(
		m3_env,
		&m3_mod,
		data.data(),
		size
	));

	assert(m3_mod != nullptr);

	check_m3("m3_LoadModule()", m3_LoadModule(
		m3_rt,
		m3_mod
	));

	check_m3("m3_LinkRawFunction(llio.printw)", m3_LinkRawFunction(
		m3_mod,
		"llio",
		"printw",
		"v(i)",
		&do_printw
	));

	check_m3("m3_LinkRawFunction(env.abort)", m3_LinkRawFunction(
		m3_mod,
		"env",
		"abort",
		"v(iiii)",
		&do_abortw
	));

	IM3Function m3_fn = nullptr;

	check_m3("m3_FindFunction()", m3_FindFunction(
		&m3_fn,
		m3_rt,
		"entry"
	));

	assert(m3_fn != nullptr);

	check_m3("m3_CallV()", m3_CallV(m3_fn));

	int32_t r = 0;
	check_m3("m3_GetResultsV()", m3_GetResultsV(m3_fn, &r));

	assert(r == 42);

	cout << "OK" << endl;

	return 0;
}

int main() {
	try {
		return pmain();
	} catch (const exception &ex) {
		cerr << "exception: " << ex.what() << endl;
		return 1;
	}
}
