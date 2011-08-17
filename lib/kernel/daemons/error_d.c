/*
 * Gurbalib error_d
 *
 * By Aidil.
 *
 */
#include <tlsvar.h>
#include <error_handling.h>

/*
 * counts the number of runtime errors we have had so far
 *
 */
int count;
int inat;

static void create() {
  DRIVER->register_error_d();
}

/*
 * NAME:        format_runtime_error()
 * DESCRIPTION: format a runtime error
 */
string format_runtime_error( string error, mixed **trace, int caught, int ticks, int atom ) {
  string result;
  string progname, objname, function, str, tmp, args;
  int i, sz, line, len;
  object player;

  result = "";

  count++;
  if(sscanf(error, MAGIC_ERROR_RETHROWN+"%s", tmp ) == 1) {
    result = tmp;
  } else if( (sz=sizeof(trace) - 1) > 0 ) {
    if( sscanf( error, MAGIC_ERROR_ARGCHECK+"%s", tmp ) == 1 && trace[sz-1][1] == AUTO && trace[sz-1][2] == "argcheck") {
      sz -= 2;
    }

    for( i=0; i<sz; i++ ) {
      progname = trace[i][1];
      function = trace[i][2];
      objname = trace[i][0];
      line = trace[i][3];
#ifdef ERROR_SHOW_ARGUMENTS
      if(sizeof(trace[i][5..]) != 0) 
        args = sizeof(trace[i][5..]) + " arguments:\n      "+dump_value( trace[i][5..], ([ ]) );
      else args = nil;
#endif
      if( line == 0 ) {
        str = "    ";
      } else {
        str = "    " + line;
        str = str[strlen( str ) - 4 ..];
      }
      str += " " + function + " ";
      len = strlen( function );
      if( len < 17 ) {
        str += "                 "[len ..];
      }
      str += progname;
      if( progname != objname ) {
        len = strlen(progname);
        if( len < strlen(objname) && progname == objname[.. len - 1]) {
          str += " (" + objname[len ..] + ")";
        } else {
          str += " (" + objname + ")";
        }
      }
#ifdef ERROR_SHOW_ARGUMENTS
      if(i > 1 && args) {
        str += ", " + args;
      }
#endif

      if(i == sz - 1) {
        result = error + "\n" + ctime(time()) + ":" + 
           "Object: " + objname + ", program: " +
           progname + ", line " + line + "\n" + result ;
      }
      result += ( str + "\n" );
    }
  } else {
    result = error + "<NO CALL TRACE AVAILABLE>\n";
  }

  if(atom >= 0) result = "[ATOMIC:"+atom+"] "+result;
  return result;
}

static void log_runtime_error(string result, int caught) {
  object player;
  string * lines, rethrown;
  int i, sz;
  mixed verbose;

  if(this_user()) {
    player = this_user()->query_player();
  }

  if(player) {
    verbose = player->query_env("verbose_errors");
    if(stringp(verbose) && (verbose == "1" || verbose == "on")) verbose = 1;
  }

  lines = explode( result, "\n" );

  if(caught) {
    if(player && player->query_name() && SECURE_D->query_wiz( player->query_name()) ) {
      mixed display_caught;

      display_caught = player->query_env("display_caught");
      if(intp(display_caught)) display_caught = (string) display_caught;

      

      switch(display_caught) {
        case "on" :
        case "1"  :
          if(verbose) {
            write( result );
          } else {
            write( lines[0]+"%^RESET%^\n" );
          }
          break;
      }
    }
  } else {
    lines = explode( result, "\n" );
    for(i = 0, sz = sizeof(lines); i < sz; i++ ) {
      console_msg(lines[i]+"\n");
    }
    write_file("/logs/errors/runtime", result+"\n");
    if(player) {
      if( SECURE_D->query_wiz( player->query_name() ) == 1 ) {
        write("%^RED%^Runtime error: %^RESET%^"+
          "%^CYAN%^"+ (verbose ? result : lines[0]) + "%^RESET%^"
        );
      } else {
        write( "You have encountered a rift in reality. Please report it to the admins.\n" );
      }
      player->write_prompt();
    }
  }
}

void runtime_error(string error, mixed **trace, int caught, int ticks, int atom) {
  string result;

  result = format_runtime_error( error, trace, caught, ticks, atom );

  if(caught) {
    DRIVER->set_tlvar(TLS_CAUGHT_ERROR, result);
  }

  /*
   * not atomic, we can write to files, do so.
   *
   */
  if(atom < 0) {
    log_runtime_error(result, caught);
  } else {
  /*
   * Writing to files is illegal here, and variables will get rolled back after the
   * error, except for output to the console, so that is what we do to leave a trace
   * of the error.
   */
    inat++;
    console_msg(result);
  }
}
    
string format_compile_error( string file, int line, string err ) {
  object player;

  if(this_user()) {
    player = this_user()->query_player();
  }
  if (player) {
  return ctime(time()) + ":" + this_player()->query_name() + ":" + file + 
     ", " + (string) line + ": " + err + "\n";
  } else {
  return ctime(time()) + ": DRIVER :" + file + 
     ", " + (string) line + ": " + err + "\n";
  }
}

void compile_error( string file, int line, string err ) {
  string error;
  object usr;
  object plr;

  error = format_compile_error( file, line, err );
  console_msg( error );

  if(!inat) write_file("/logs/errors/compile", error);
  else console_msg("Compile error during atomic execution.\n");

  usr = this_user();
  if( usr ) {
    if( usr->query_player() ) {
      if( SECURE_D->query_wiz( usr->query_player()->query_name() ) == 1 ) {
        write( "%^RED%^Compiler error:%^RESET%^\n"+
               "%^YELLOW%^"+ error + "%^RESET%^" 
        );
      } else {
        write( "You have encountered a rift in reality. Please report it to the admins.\n" );
      }
    }
  }
}


