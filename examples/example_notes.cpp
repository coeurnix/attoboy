#include <attoboy/attoboy.h>

using namespace attoboy;

Path getNotesFile() {
  Path appData = Path::GetLocalAppDirectory();
  Path notesDir(appData.toString() + "\\MiniNotes");
  notesDir.makeDirectory();
  return Path(notesDir.toString() + "\\notes.json");
}

List loadNotes() {
  Path notesFile = getNotesFile();
  if (notesFile.exists()) {
    String json = notesFile.readToString();
    return List::FromJSONString(json);
  }
  return List();
}

void saveNotes(List notes) {
  Path notesFile = getNotesFile();
  String json = notes.toJSONString();
  notesFile.writeFromString(json);
}

void atto_main() {
  Console con;
  List notes = loadNotes();

  con.println("Mini Notes", CON_CYAN);
  con.println(String("-").repeat(40), CON_DARK_GRAY);

  ConsoleInput inputOptions;
  inputOptions.completions = List("add", "list", "delete", "quit");
  inputOptions.history = List();

  bool running = true;
  while (running) {
    String cmd = con.input("> ", inputOptions);

    if (cmd == "quit" || cmd.isEmpty()) {
      running = false;
    } else if (cmd == "add") {
      String note = con.input("Note: ");
      if (!note.isEmpty()) {
        Map entry("text", note, "created", DateTime().toString());
        notes.append(entry);
        saveNotes(notes);
        con.println("Note added!", CON_GREEN);
      }
    } else if (cmd == "list") {
      if (notes.isEmpty()) {
        con.println("No notes yet.", CON_DARK_GRAY);
      } else {
        for (int i = 0; i < notes.length(); i++) {
          Map note = notes.at(i);
          String text = note.get("text", "");
          con.println(String(i + 1, ". ", text));
        }
      }
    } else if (cmd.startsWith("delete ")) {
      String numStr = cmd.substring(7);
      int idx = numStr.toInteger() - 1;
      if (idx >= 0 && idx < notes.length()) {
        notes.remove(idx);
        saveNotes(notes);
        con.println("Note deleted.", CON_YELLOW);
      } else {
        con.println("Invalid note number.", CON_RED);
      }
    } else {
      con.println("Commands: add, list, delete <num>, quit", CON_DARK_GRAY);
    }
  }

  con.println("Goodbye!", CON_CYAN);
  Exit();
}
