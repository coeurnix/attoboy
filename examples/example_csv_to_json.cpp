//==============================================================================
// example_csv_to_json.cpp - CSV to JSON Converter
//==============================================================================
// Converts a CSV file to JSON format. The first row is used as field names.
//
// Usage:
//   example_csv_to_json <input.csv>              - Print JSON to console
//   example_csv_to_json <input.csv> <output.json> - Save JSON to file
//
// Example CSV:
//   name,age,city
//   Alice,30,New York
//   Bob,25,Boston
//
// Output JSON:
//   [{"name":"Alice","age":"30","city":"New York"},
//    {"name":"Bob","age":"25","city":"Boston"}]
//==============================================================================

#include "attoboy/attoboy.h"

using namespace attoboy;

extern "C" void atto_main() {
  // Set up command-line arguments
  Arguments args;
  args.addPositionalParameter("input", "Input CSV file")
      .addPositionalParameter("output",
                              "Output JSON file (prints to console if omitted)")
      .requireArgument("input")
      .setHelp("example_csv_to_json - CSV to JSON Converter\n\n"
               "Converts CSV files to JSON arrays of objects.\n"
               "The first row of the CSV becomes the field names.\n\n"
               "Usage: example_csv_to_json <input.csv> [output.json]");

  // Parse arguments
  Map parsed = args.parseArguments();
  if (parsed.isEmpty()) {
    return;
  }

  // Get file paths
  String inputPath = parsed.get<String, String>("input");
  String outputPath = parsed.get<String, String>("output");

  // Check input file exists
  Path inputFile(inputPath);
  if (!inputFile.exists()) {
    LogError("Input file not found: ", inputPath);
    Exit(1);
    return;
  }

  // Read the CSV file
  String csvContent = inputFile.readToString();
  if (csvContent.isEmpty()) {
    LogError("Failed to read input file (or file is empty)");
    Exit(1);
    return;
  }

  // Parse CSV into a list of lists (rows of cells)
  List csvData = List::FromCSVString(csvContent);
  if (csvData.isEmpty()) {
    LogError("Failed to parse CSV data");
    Exit(1);
    return;
  }

  // Need at least a header row
  if (csvData.length() < 1) {
    LogError("CSV file has no data");
    Exit(1);
    return;
  }

  // First row contains column headers
  List headers = csvData.at<List>(0);
  if (headers.isEmpty()) {
    LogError("CSV header row is empty");
    Exit(1);
    return;
  }

  // Build JSON array of objects
  List jsonArray;

  // Process each data row (starting from row 1, skipping headers)
  for (int rowIndex = 1; rowIndex < csvData.length(); rowIndex++) {
    List row = csvData.at<List>(rowIndex);

    // Create an object for this row
    Map rowObject;

    // Map each column to its header name
    for (int colIndex = 0; colIndex < headers.length(); colIndex++) {
      String headerName = headers.at<String>(colIndex);

      // Get cell value (empty string if row is shorter than headers)
      String cellValue;
      if (colIndex < row.length()) {
        cellValue = row.at<String>(colIndex);
      }

      rowObject.put(headerName, cellValue);
    }

    jsonArray.append(rowObject);
  }

  // Convert to JSON string
  String jsonOutput = jsonArray.toJSONString();

  // Output result
  if (outputPath.isEmpty()) {
    // Print to console
    Log(jsonOutput);
  } else {
    // Write to file
    Path outputFile(outputPath);
    if (outputFile.writeFromString(jsonOutput)) {
      Log("Converted ", csvData.length() - 1, " records");
      Log("Saved to: ", outputPath);
    } else {
      LogError("Failed to write output file: ", outputPath);
      Exit(1);
    }
  }
}