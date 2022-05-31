inherit "/std/modules/m_save_restore";

#define SAVE_FUNCTIONS ({ "query_real_ids", "query_adjs", "query_short", "query_long", "query amount", "query_quality", "query_stored_energy", "query_recipe", "query_stage", "query_quality_points", "query_resource_quality", "query_fuel", "query_time", "query_attributes", "query_weight", "query_internal_max_weight" })

#define LOAD_FUNCTIONS ({ "set_id_array", "set_adj_array", "set_short", "set_long", "set_amount", "set_quality", "set_stored_energy", "set_recipe", "set_stage", "set_quality_points", "set_resource_quality", "set_fuel", "set_time", "set_attributes", "set_weight", "set_internal_max_weight" })
   
mixed *autoload_string;

string *query_autoload_string(void) {
   return autoload_string;
}

void set_autoload_string(string *str) {
   autoload_string = str;
}

string save_variables(object obj) {
   string ret;
   int size, a;

   ret = "";
   size = sizeof(SAVE_FUNCTIONS);

   for (a = 0; a < size; a++) {
      ret += save_variable(call_other(obj, SAVE_FUNCTIONS[a]));
      if ((a + 1) != size) {
         ret += "^!";
      }
   }
   return ret;
}

void clone_autoload_objects(void) {
   string *vars;
   int a, size, b, sizeb, num;
   object *ob;
   mixed objs;

   objs = autoload_string;
   ob = ({ });

   if (!autoload_string) {
      return;
   }

   size = sizeof(objs);
   for (a = 0; a < size; a++) {
      vars = explode(objs[a], "^!");
      if(file_exists(vars[0])){
         ob += ({ clone_object(vars[0]) });
         ob[a]->setup();
         ob[a]->move(this_object());
         if (vars[1] == "wield") {
            this_player()->do_wield(ob[a], explode(vars[2], ","));
         } else if (vars[1] == "wear") {
            this_player()->do_wear(ob[a], explode(vars[2], ","));
         }
         sizeb = sizeof(LOAD_FUNCTIONS);
         for (b = 0; b < sizeb; b++) {
            call_other(ob[a], LOAD_FUNCTIONS[b], restore_variable(vars[b + 3]));
         }
      }
   }
   for (a = 0; a < size; a++) {
      vars = explode(objs[a], "^!");
      num = str2val(vars[sizeb + 3]);
      if (num > -1) {
         ob[a]->move(ob[num]);
      }
   }
}

void compose_autoload_string(void) {
   object ob;
   string tmp, action, worns;
   object *inv;
   int i, max, environment;

   autoload_string = ({ });

   worns = "";
   inv = query_deep_inventory(this_object());
   max = sizeof(inv);
   for (i = 0; i < max; i++) {
      tmp = inv[i]->query_autoload_filename();
      if (inv[i]->query_worn()) {
         action = "wear";
      } else if (inv[i]->query_wielded()) {
         action = "wield";
      } else {
         action = "none";
      }
      if (inv[i]->query_worn_on()) {
         worns = implode(inv[i]->query_worn_on(),",");
      } else {
         worns = "none";
      }
      if (tmp && (tmp != "")) {
         if (inv[i]->query_environment() != this_object()) {
            environment = member_array(inv[i]->query_environment(), inv);
         } else {
            environment = -1;
         }
         autoload_string += ({ tmp + "^!" + action + "^!" + worns + "^!" + save_variables(inv[i]) + "^!" + environment });
      }
   }
}
