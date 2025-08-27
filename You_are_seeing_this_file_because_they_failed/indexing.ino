void indexing() {
  byte pointer = 0;
  for (byte i = 0; i < 10; i++) {
    if (vehicle_index[0][i] == 0 || vehicle_index[0][i] == vehicle.id) {
      pointer = i;
      break;
    }
  }

  vehicle_index[0][pointer] = vehicle.id;
  vehicle_index[1][pointer] = vehicle.b;

  if (vehicle_index[1][pointer] == 0) {
    for (byte i = pointer; i < 9; i++) {
      vehicle_index[0][i] = vehicle_index[0][i + 1];
      vehicle_index[1][i] = vehicle_index[1][i + 1];
    }
  }

  vehicle_sum = 0;
  for (byte i = 0; i < 10; i++) vehicle_sum += vehicle_index[1][i];
}