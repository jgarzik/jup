#!/usr/bin/env node

const fs = require('fs');
const { exec } = require('child_process');

if (!process.env.srcdir) {
	console.error("No $srcdir provided, aborting.");
	process.exit(1);
}

const jupPath = "./jup";
const dataDir = process.env.srcdir + "/test/data/";

function runTest(testInfo)
{
	var cmdArgs = '';
	if ('cmd' in testInfo) {
		cmdArgs = ' ' + fs.readFileSync(dataDir + testInfo.cmd,'utf8').trim();
	}
	const cmd = `cat ${dataDir}${testInfo.in} | ${jupPath}${cmdArgs}`;
	const expectedOutput = fs.readFileSync(dataDir + testInfo.out, 'utf8');
	// console.log(cmd);

	const logPrefix = `${testInfo.desc}: `;

	exec(cmd, (err, stdout, stderr) => {
		if (err) {
			console.error(err);
			console.error(logPrefix + 'failed');
			process.exit(1);
		}

		if (stderr) {
			console.error("Unexpected stderr output: " + stderr);
			console.error(logPrefix + 'failed');
			process.exit(1);
		}
		if (stdout !== expectedOutput) {
			console.error("Unmatched stdout output");
			console.error("EXPECTED:" + expectedOutput);
			console.error("ACTUAL:" + stdout);
			console.error(logPrefix + 'failed');
			process.exit(1);
		}

		console.log(logPrefix + 'ok');
	});
}

function runTests()
{
	const testList = JSON.parse(fs.readFileSync(dataDir+'all-tests.json'));

	const testCount = testList.length;
	console.log(`Running ${testCount} tests:`);

	testList.forEach(function(testInfo) {
		runTest(testInfo);
	});
}

runTests();

