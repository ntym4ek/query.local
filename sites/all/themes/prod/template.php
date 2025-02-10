<?php

/**
 * @file
 * Bootstrap sub-theme.
 *
 * Place your custom PHP code in this file.
 */

/**
 * Implements hook_theme().
 */
function prod_theme()
{
  return [
    'nomenklatura_teaser' => [
      'variables' => ['nom_info' => null, 'month_start' => null, 'message_info' => null, 'plan_rec_info' => null, 'warning' => null],
    ],
    'message' => [
      'variables' => ['message_info' => null],
    ],
    'message_mail' => [
      'variables' => ['message_info' => null],
    ],
    'produce_unit' => [
      'variables' => ['produce_unit' => NULL, 'month_start' => null, 'show_amount' => null, 'show_nomenklatura' => null, 'show_month' => null],
    ],
    'field_custom' => [
      'variables' => ['title' => null, 'content' => null],
    ],
  ];
}

/**
 * Функция темизации Сообщения
 * @param $vars
 * @return string
 */
function prod_message($vars)
{

  if (in_array($vars['message_info']['author'], [PROD_MESSAGE_AUTHOR_CLIENT, PROD_MESSAGE_AUTHOR_SYS_CLIENT])) {
    $row_class = ' m-client';
  } else {
    $row_class = ' m-prod';
  }

  // подготовить файлы
  $files = '';
  if ($vars['message_info']['files']) {
    $arr = [];
    foreach ($vars['message_info']['files'] as $file) {
      $arr[] = '<a class="" href="' . $file['url'] . '" title="Скачать файл" download>' . $file['name'] . '</a>';
    }
    $files = implode(', ', $arr);
  }


  $message_class = '';

  $output  = "<div class=\"message-row$row_class\">";
  $output .=   "<div class=\"message$message_class\">";
  if ($vars['message_info']['changes']['formatted'])
    $output .=     '<div class="m-change">' . $vars['message_info']['changes']['formatted'] . '</div>';
  if ($vars['message_info']['reason'])
    $output .=     '<div class="m-reason">' . $vars['message_info']['reason'] . '</div>';
  if ($vars['message_info']['comment'])
    $output .=     '<div class="m-comment">' . $vars['message_info']['comment'] . '</div>';
  if ($files) {
    $output .=     '<div class="m-files">';
    $output .=       $files;
    $output .=     '</div>';
  }
  if ($vars['message_info']['warning']) {
    $output .=     '<div class="m-warning"></div>';
  }


  if (true) {
    // вывести отладочную информацию
    $output .=      '<div class="m-date"><a title="id: ' . $vars['message_info']['id'] . '">' .
                      date('d.m.Y H:i', $vars['message_info']['created']) .
                    '</a></div>';
  } else {
    $output .=     '<div class="m-date">' . date('d.m.Y H:i', $vars['message_info']['created']) . '</div>';
  }
  $output .=   '</div>';
  $output .= '</div>';

  return $output;
}

/**
 * Функция темизации Сообщения для письма (с встроенными стилями)
 * @param $vars
 * @return string
 */
function prod_message_mail($vars)
{
  // подготовить файлы
  $files = '';
  if ($vars['message_info']['files']) {
    $arr = [];
    foreach ($vars['message_info']['files'] as $file) {
      $arr[] = '<a style="text-decoration: none;color: inherit;font-size: 12px;" href="' . url($file['url'], ['absolute' => true]) . '" title="Скачать файл" download>' . $file['name'] . '</a>';
    }
    $files = implode(', ', $arr);
  }

  $changes_formatted = str_replace('class="nowrap"', 'style="white-space: nowrap;"', $vars['message_info']['changes']['formatted']);

  $output  =   '<div class="message" style="font-size: 14px;color: #333333;flex: 0 1 auto;background: #eee;border-radius: 5px;max-width: 500px; min-width: 150px;padding: 1.5rem 2rem 0.7rem; margin-bottom: 1rem;">';
  if ($changes_formatted)
    $output .=     '<div class="m-change" style="font-size: 18px;font-weight: 600;">' . $changes_formatted . '</div>';
  if ($vars['message_info']['reason'])
    $output .=     '<div class="m-reason" style="color: #999999;margin-bottom: 1rem;">' . $vars['message_info']['reason'] . '</div>';
  if ($vars['message_info']['comment'])
    $output .=     '<div class="m-comment" style="">' . $vars['message_info']['comment'] . '</div>';
  if ($files) {
    $output .=     '<div class="m-files" style="background: #dddddd;border-radius: 5px;padding: 2px 10px;margin-top: 5px;">';
    $output .=       $files;
    $output .=     '</div>';
  }

  $output .=     '<div class="m-date" style="color: #999999;font-size: 12px;text-align: right; margin-top:0.5rem;">' . date('d.m.Y H:i', $vars['message_info']['created']) . '</div>';
  $output .=   '</div>';

  return $output;
}

function prod_nomenklatura_teaser($vars)
{
  $output = '';
  $title        = theme('field_custom', ['title' => 'Номенклатура', 'content' => $vars['nom_info']['label']]);
  $volume       = $vars['plan_rec_info'] ? theme('field_custom', ['title' => 'Объём выпуска, л(кг)', 'content' => helper_number_format($vars['plan_rec_info']['volume'], 2, ' ')]) : '';
  $date         = !empty($vars['plan_rec_info']['date']) ? theme('field_custom', ['title' => 'Начало выпуска', 'content' => date('d.m.Y', $vars['plan_rec_info']['date'])]) : '';

  $date_change  = theme('field_custom', ['title' => 'Дата изменения', 'content' => date('d.m.Y', $vars['message_info']['created'])]);
  $change       = theme('field_custom', ['title' => 'Последнее изменение', 'content' => $vars['message_info']['changes']['formatted']]);

  $url          = '/production/nomenklatura/' . $vars['nom_info']['id'] . '/' . $vars['month_start'];
  $button       = '<a class="btn btn-default btn-xs" href="' . $url . '">подробнее</a>';

  $class = '';
  if (in_array($vars['message_info']['author'], [PROD_MESSAGE_AUTHOR_CLIENT, PROD_MESSAGE_AUTHOR_SYS_CLIENT])) {
    $class = ' m-client';
  }
  if (in_array($vars['message_info']['author'], [PROD_MESSAGE_AUTHOR_PROD, PROD_MESSAGE_AUTHOR_SYS_PROD])) {
    $class = ' m-prod';
  }

  $class_warn = '';
  if (!empty($vars['warning'])) {
    if ($vars['warning'] == PROD_WARNING_YELLOW) $class_warn = ' warn-yellow';
    if ($vars['warning'] == PROD_WARNING_RED) $class_warn = ' warn-red';
  }


  $output .='<a href="">';
  $output .=  '<div class="nom-item' . $class_warn . '">';
  $output .=    '<div class="n-header">';
  $output .=      '<div class="n-title"><a href="' . $url . '">' . $title . '</a></div>';
  $output .=      '<div class="n-volume">' . $volume . ' </div>';
  $output .=      '<div class="n-date">' . $date . '</div>';
  $output .=    '</div>';
  $output .=    '<div class="n-footer">';
  $output .=      '<div class="n-date-change">' . $date_change . '</div>';
  $output .=      '<div class="n-change' . $class . '">' . $change . ' </div>';
  $output .=      '<div class="n-actions">' . $button . ' </div>';
  $output .=    '</div>';
  $output .=  '</div>';
  $output .='</a>';

  return $output;
}

/**
 * Функция темизации производственной Установки
 */
function prod_produce_unit($vars)
{
  $month_start = $vars['month_start'];
  $produce_unit = $vars['produce_unit'];
  $pu_name = $produce_unit['info']['label'];
  $load = $produce_unit['load'];

  $days = '';
  for ($i = 1; $i <= date("t", $month_start); $i++) {
    $day_start = $month_start + ($i - 1) * 60 * 60 * 24;

    $classes = []; $tooltip = '';
    if (!empty($load[$day_start])) {
      $classes = $load[$day_start]['classes'];
      $nam_name = $produce_unit['nom'][$load[$day_start]['nom_id']]['info']['label'];

      $title = str_replace('"', '', $nam_name) . ($load[$day_start]['output'] ? '<br />' . $load[$day_start]['output'] : '');
      $tooltip = ' data-toggle="tooltip" data-placement="top" data-html="true" title="' . $title . '"';
      $tooltip = empty($load[$day_start]['is_own']) ? '' : $tooltip;
    }
    $days .= '<span class="c-box ' . implode(' ', $classes) . '"' . $tooltip . '>' . $i . '</span>';
  }

  $month_label = empty($vars['show_month']) ? 'Даты' : helper_month_label_ru(date('n', $month_start)) . ' ' . date('Y', $month_start);

  $output =
    '<div class="produce-unit" data-putid="' . $produce_unit['info']['id'] . '">' .
      '<div class="produce-unit-name">' .
        '<label class="label" for="edit-name">Установка</label>' .
        '<h3>' . $pu_name . '</h3>' .
      '</div>' .
      '<div class="produce-unit-dates">' .
        '<label class="label">' . $month_label . '</label>' .
        '<div class="form-checkboxes">' .
        $days .
        '</div>' .
      '</div>';

  if (!empty($vars['show_amount'])) {
    $output .=
      '<div class="produce-unit-aux">' .
        '<label class="label">Объём выпуска</label>' .
        '<h3>' . number_format($produce_unit['amount'], 0, ',', ' ') . '</h3>' .
      '</div>';
  }
  if (!empty($vars['show_nomenklatura'])) {
    $output .=
      '<div class="produce-unit-nom">' .
        implode('; ', $produce_unit['nom']['list']) .
      '</div>';
  }
  $output .= '</div>';


  return $output;
}

function prod_field_custom($vars)
{
  $output =   '<div class="field field-' . drupal_strtolower(transliteration_get($vars['title'])) . '">';
  $output .=    '<label class="field-label">' . $vars['title'] . '</label>';
  $output .=    '<div class="field-content">' . $vars['content'] . '</div>';
  $output .=  '</div>';

  return $output;
}

function prod_preprocess_mimemail_message(&$vars)
{
  // переменные для шаблона письма
  // logo для писем (берём лого из текущей темы, если существует)
  $path = path_to_theme() . '/images/logo/logo_mail.png';
  $vars['logo_mail'] = file_exists($path) ? file_create_url($path) : theme_get_setting('logo');
  $site_name  = 'Кирово-Чепецкий завод «Агрохимикат»';
  $vars['site_name'] = $site_name;
  // подпись на языке письма
  $vars['sign']   = empty($vars['message']['params']['context']['sign']) ? t('Postal robot') . ' ' . t($site_name) : $vars['message']['params']['context']['sign'];
  // notice - текст сообщения о том, что письмо сформировано автоматически
  $vars['notice'] = !isset($vars['message']['params']['context']['auto']) ? t('This message was generated automatically and does not require a response') : $vars['message'] ['params']['context']['auto'];
}
