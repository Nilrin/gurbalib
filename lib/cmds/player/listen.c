inherit M_COMMAND;

void usage() {
   string *lines;

   lines = ({ "Usage: listen [-h] [OBJECT]" });
   lines += ({ "" });
   lines += ({ "Listen to the specified object." });
   lines += ({ "" });
   lines += ({ "Options:" });
   lines += ({ "\t-h\tHelp, this usage message." });
   lines += ({ "Examples:" });
   lines += ({ "\tlisten door" });
   lines += ({ "\tlisten" });
   lines += ({ "See also:" });
   lines += ({ "\tbuy, identify, list, look, sell, value" });
   if (query_wizard(this_player())) {
      lines += ({ "\tlast, locate, mudlist, possess, rwho, snoop, where, who" 
      });
   }

   this_player()->more(lines);
}

static void main(string str) {
   object obj;

   if (!str || (str == "")) {
      if (!this_player()->query_environment()->do_listen()) {
         this_player()->query_environment()->tell_room(this_player(),
           this_player()->query_Name() + "Pauses to listen closely.\n");
         write("You hear nothing.\n");
      } 
      return;
   }

   if (sscanf(str, "-%s", str)) {
      usage();
      return;
   }

   obj = this_player()->present(lowercase(str));
   if (!obj) {
      write("Listen to what?\n");
      return;
   }

   if (!obj->do_listen()) {
      this_player()->query_environment()->tell_room(this_player(),
         this_player()->query_Name() + "Pauses to listens to " + 
         obj->query_id() + "\n");
      write("You hear nothing.\n");
      return;
   }
}
