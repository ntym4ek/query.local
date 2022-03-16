<?php


function warehouse_process_incoming_files()
{
  $directory = 'private://exchange/';

  // -- Заявки на Погрузку
  // получить список файлов по шаблону
  $mask = '/^wwork_import.*\.json/';
  $files = file_scan_directory($directory, $mask, ['recurse' => false], 0);

  // импортировать
  if ($files) {
    foreach($files as $file) {
      $array = drupal_json_decode(file_get_contents($file->uri));
      if (warehouse_wwork_import($array)) {
        $path = $directory . 'success/' . date('Y') . '/' . date('m') . '/' . date('d');
      } else {
        $path = $directory . 'fail/' . date('Y') . '/' . date('m') . '/' . date('d');
      }
      file_prepare_directory($path, FILE_CREATE_DIRECTORY);
      file_unmanaged_move($file->uri, $path . '/' . $file->filename);
    }
  }
}

function warehouse_wwork_import($array)
{
  $is_ok = false;
  if (is_array($array) && !empty($array['id'])) {
    $wwork_wr = entity_metadata_wrapper('warehouse_work', $array['id']);

    if ($wwork_wr->value()) {
      // статус
      foreach(['checkin', 'onload', 'gone'] as $status) {
        $status_key = $status . '_time';
        if (!empty($array[$status_key])) {
          $status_updated = trim($array[$status_key]);
          $date = explode(' ', $status_updated)[0];
          $date_arr = explode('.', $date);
          $time = explode(' ', $status_updated)[1];
          $time_arr = explode(':', $time);
          $update_unix = gmmktime($time_arr[0], $time_arr[1], 0, $date_arr[1], $date_arr[0], $date_arr[2]);
          if ($status == 'checkin') $wwork_wr->field_checkin_time = $update_unix;
          if ($status == 'onload') $wwork_wr->field_onload_time = $update_unix;
          if ($status == 'gone') $wwork_wr->field_gone_time = $update_unix;
          $is_ok = true;
        }
      }
      // или обработка
      if (!empty($array['processing'])) {
        $wwork_wr->field_processing = $array['processing'];
        $is_ok = true;
      }
      // или отмена
      if (!empty($array['cancel_reason'])) {
        $wwork_wr->field_cancel_reason = $array['cancel_reason'];
        $is_ok = true;
      }

      if ($is_ok) {
        $wwork_wr->save();
      }
    }
  }

  if ($is_ok) {
    watchdog('import', 'Заявка на Погрузку ID ' . $array['id'] . ' обновлена.', [], WATCHDOG_INFO);
    return true;
  } else {
    watchdog('import', 'Заявка на Погрузку не обновлена.', [], WATCHDOG_ERROR);
    return false;
  }
}

