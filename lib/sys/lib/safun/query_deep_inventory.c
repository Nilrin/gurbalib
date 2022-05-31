
private object *deep_inventory;

private add_objects(object obj) {
   object *objs;
   int a, size;

   deep_inventory += ({ obj });
   objs = obj->query_inventory();
   if (objs) {
      size = sizeof(objs);
      for (a = 0; a < size; a++) {
         add_objects(objs[a]);
      }
   }
}

object *query_deep_inventory(object container) {
   object objs;
   int a, size;

   if (!deep_inventory) {
      deep_inventory = ({ });
   }
   add_objects(container);
   deep_inventory -= ({ container });
   return deep_inventory;
}
