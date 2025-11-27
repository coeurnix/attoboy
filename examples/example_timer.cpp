//==============================================================================
// example_timer.cpp - Command Execution Timer
//==============================================================================
// Runs another command and reports exactly how long it took to execute.
//
// Usage:
//   example_timer "command arg1 arg2 ..."
//
// Example:
//   example_timer "gcc -o hello hello.c"
//   example_timer "python script.py"
//   example_timer "dir /s"
//
// The output shows the command's normal output, then the timing results.
//==============================================================================

#include "attoboy/attoboy.h"

using namespace attoboy;

extern "C" void atto_main() {
  // Set up command-line arguments
  Arguments args;
  args.addPositionalParameter("command",
                              "Command line to execute (quote if has spaces)")
      .requireArgument("command")
      .setHelp("example_timer - Command Execution Timer\n\n"
               "Runs a command and reports how long it took.\n\n"
               "Usage: example_timer \"<command> [args...]\"\n\n"
               "Examples:\n"
               "  example_timer \"gcc -o hello hello.c\"\n"
               "  example_timer \"python myscript.py\"\n"
               "  example_timer \"dir /s /b\"\n"
               "  example_timer notepad.exe");

  // Parse arguments
  Map parsed = args.parseArguments();
  if (parsed.isEmpty()) {
    Exit(1);
    return;
  }

  // Get the command to run
  String command = parsed.get<String, String>("command");
  if (command.trim().isEmpty()) {
    LogError("No command specified. Use -h for help.");
    Exit(1);
  }

  // Display what we're running
  Log("Running: ", command);
  Log(String("-").repeat(60));

  // Record start time
  DateTime startTime;

  // Run the command via cmd.exe /c
  // This allows running any command including built-in shell commands
  Path cmdExe("cmd.exe");
  int exitCode = Subprocess::Run(cmdExe, "/c", command);

  // Record end time
  DateTime endTime;

  // Calculate duration in milliseconds
  long long durationMs = endTime.diff(startTime);

  // Convert to readable format
  int totalSeconds = (int)(Math::Div64(durationMs, 1000LL));
  int milliseconds = (int)(Math::Mod64(durationMs, 1000LL));
  int hours = totalSeconds / 3600;
  int minutes = (totalSeconds % 3600) / 60;
  int seconds = totalSeconds % 60;

  // Display results
  Log(String("-").repeat(60));
  Log("Exit code: ", exitCode);
  Log();

  // Show timing in multiple formats for clarity
  Log("Duration:");

  // Always show milliseconds
  Log("  ", durationMs, " ms");

  // Show seconds with decimal if >= 1 second
  if (durationMs >= 1000) {
    // Format as seconds with 3 decimal places
    int wholeSeconds = (int)(Math::Div64(durationMs, 1000));
    int fracMs = (int)(Math::Mod64(durationMs, 1000));

    // Pad milliseconds to 3 digits
    String msStr;
    if (fracMs < 10) {
      msStr = String("00", fracMs);
    } else if (fracMs < 100) {
      msStr = String("0", fracMs);
    } else {
      msStr = String(fracMs);
    }

    Log("  ", wholeSeconds, ".", msStr, " seconds");
  }

  // Show hours:minutes:seconds if >= 1 minute
  if (totalSeconds >= 60) {
    // Pad minutes and seconds to 2 digits
    String minStr = (minutes < 10) ? String("0", minutes) : String(minutes);
    String secStr = (seconds < 10) ? String("0", seconds) : String(seconds);

    if (hours > 0) {
      Log("  ", hours, ":", minStr, ":", secStr);
    } else {
      Log("  ", minutes, ":", secStr);
    }
  }

  // Exit with the same code as the command
  if (exitCode != 0) {
    Exit(exitCode);
  }
}
