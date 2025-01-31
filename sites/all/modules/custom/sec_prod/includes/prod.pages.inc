<?php

function prod_nom_list_form($form, $form_values)
{
  $month = empty($form_values["values"]["month"]) ? date('n') : $form_values["values"]['month'];
  $year = empty($form_values["values"]['year']) ? date('Y') : $form_values["values"]['year'];

  $button_add = '<a class="btn btn-primary autodialog" href="/production/message/add">Новая заявка на выпуск</a>';

  $sort_by = $form_values['values']['sortby'] ?? 'date';
  if (!empty($form_values["triggering_element"]) && in_array($form_values["triggering_element"]["#name"], ['abc', 'date'])) {
    $sort_by = $form_values["triggering_element"]["#name"];
  }

  $form = [
    '#prefix' => '<div id="nom-list-form-wrapper" class="nom-list-page">' .
                    '<div class="nom-add">' .
                      $button_add .
                    '</div>',
    '#suffix' => '</div>',
    'filters' => [
      '#type' => 'container',
      '#attributes' => ['class' => ['filters']],
    ],
    'sortby' => ['#type' => 'hidden', '#value' => $sort_by],
  ];

  $month_opts = [];
  for($i = 1; $i <= 12; $i++) {
    $month_opts[$i] = helper_month_label_ru($i);
  }
  $form['filters']['month'] = [
    '#title' => 'Месяц',
    '#type' => 'select',
    '#options' => $month_opts,
    '#default_value' => [$month],
    '#ajax' => [
      'callback' => 'prod_nom_list_form_callback',
      'wrapper' => 'nom-list-form-wrapper',
    ],
    '#prefix' => '<div class="month-select-wrapper filter">',
  ];
  $year_opts = [];
  for($i = -1; $i <= 1; $i++) {
    $yr = date('Y') + $i;
    $year_opts[$yr] = $yr;
  }
  $form['filters']['year'] = [
    '#title' => 'Год',
    '#type' => 'select',
    '#options' => $year_opts,
    '#default_value' => [$year],
    '#ajax' => [
      'callback' => 'prod_nom_list_form_callback',
      'wrapper' => 'nom-list-form-wrapper',
    ],
    '#suffix' => '</div>',
  ];

  $options = [];
  if ($companies = ext_user_get_companies_by_user()) {
    foreach ($companies as $tid => $company) {
      $options[$tid] = $company['name'];
    }
  }

  $company_default = $form_values["values"]['company'] ?? array_key_first($options);

  $form['filters']['company'] = [
    '#title' => 'Компания',
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => $company_default,
    '#required' => true,
    '#ajax' => [
      'callback' => 'prod_nom_list_form_callback',
      'wrapper' => 'nom-list-form-wrapper',
    ],
    '#prefix' => '<div class="filter">',
    '#suffix' => '</div>',
  ];

  $form['filters']['abc'] = [
    '#type' => 'button',
    '#value' => 'по алфавиту',
    '#name' => 'abc',
    '#ajax' => [
      'callback' => 'prod_nom_list_form_callback',
      'wrapper' => 'nom-list-form-wrapper',
    ],
    '#attributes' => ['class' => ['btn-link', $sort_by == 'abc' ? 'selected' : '']],
    '#prefix' => '<div class="sort">',
  ];
  $form['filters']['datetime'] = [
    '#type' => 'button',
    '#value' => 'по дате изменения',
    '#name' => 'date',
    '#ajax' => [
      'callback' => 'prod_nom_list_form_callback',
      'wrapper' => 'nom-list-form-wrapper',
    ],
    '#attributes' => ['class' => ['btn-link', $sort_by == 'date' ? 'selected' : '']],
    '#suffix' => '</div>',
  ];

  // подготовить массив с инфо о Номенклатуре и изменениями плана
  $arr = $sort_arr = [];
  $month_start = gmmktime(0, 0, 0, $month, 1, $year);
  $nom_list = prod_nomenklatura_get_list_by_messages(['company_ids' => [$company_default], 'month_ts' => $month_start]);
  foreach($nom_list as $nomid => $nom) {
    $nom_info = prod_nomenklatura_get_info($nomid);
    $arr[$nomid] = [
      'nom' => $nom_info,
      'message' => null,
      'plan_rec' => null,
    ];

    if ($message_last = prod_message_get_last($month_start, $nomid)) {
      $arr[$nomid]['message'] = prod_message_get_info($message_last->id);
    }
    if ($plan_rec_last = prod_plan_record_get_last($month_start, $nomid)) {
      $arr[$nomid]['plan_rec'] = prod_plan_record_get_info($plan_rec_last->id);
    }

    $sort_key = ($sort_by == 'abc') ? $nom_info['label'] : $arr[$nomid]['message']['created'];
    $sort_arr[$sort_key] = $nomid;
  }

  // отсортировать в зависимости от заданного пользователем
  if ($sort_by == 'abc') ksort($sort_arr); else krsort($sort_arr);

  // вывести список
  $output = '<div class="nom-list">';
  foreach ($sort_arr as $key) {
    $output .= theme('nomenklatura_teaser', ['nom_info' => $arr[$key]['nom'], 'message_info' => $arr[$key]['message'], 'plan_rec_info' => $arr[$key]['plan_rec'], 'month_start' => $month_start]);
  }
  $output .= '</div>';

  $form['list'] = [
    '#markup' => $output,
  ];

  return $form;
}
function prod_nom_list_form_callback($form, $form_values)
{
  return $form;
}

function prod_nom_page($nom_id, $month_start)
{
  drupal_set_title('');
  $output = '';

  $nom_info = prod_nomenklatura_get_info($nom_id);
  $volume = null;
  $date_start = 0;

  // собрать актуальную информацию о состоянии плана производства
  $output_pu = '';
  if ($plan_rec = prod_plan_record_get_last($month_start, $nom_id)) {
    $plan_rec_info = prod_plan_record_get_info($plan_rec->id);

    if ($plan_rec_info['load']) {
      $volume = 0;
      $output_pu = '<div class="produce-units">' .
                      '<div class="produce-units-list">';
      $produce_units = prod_produce_unit_get_load($month_start, null, $plan_rec->id);
      foreach ($produce_units as $pu) {
        $volume += $pu['amount'];
        $output_pu .= theme('produce_unit', ['produce_unit' => $pu, 'month_start' => $month_start, 'show_amount' => true, 'show_nomenklatura' => false]);
      }
      $output_pu .= '</div></div>';
      $output_pu .=  '<div class="produce-unit-legenda bottom"><div><span></span>- установка свободна</div><div><span class="loaded"></span>- установка занята</div><div><span class="company-loaded"></span>- другая продукция компании</div><div><span class="selected"></span>- даты производства</div></div>';
    }

    $date_start = $plan_rec_info['date_start'];
  }
  $title = theme('field_custom', ['title' => 'Номенклатура', 'content' => $nom_info['label']]);
  $volume     = $plan_rec ? theme('field_custom', ['title' => 'Объём выпуска л(кг)', 'content' => helper_number_format($volume, 2, ' ')]) : '';
  $date_start = $plan_rec && $date_start ? theme('field_custom', ['title' => 'Начало выпуска', 'content' => date('d.m.Y', $date_start)]) : '';


  // вывести историю сообщений
  $output_m = '';
  if ($messages = prod_message_get_all($month_start, $nom_id)) {
    foreach ($messages as $message) {
      $message_info = prod_message_get_info($message->id);
      $output_m .= theme('message', ['message_info' => $message_info]);
    }
  }

  // определить доступные Действия
  $url = '/production/message/add/' . $nom_id . '/' . $month_start;
  $output_a  = '<a class="btn btn-primary autodialog" href="' . $url . '?subject=date">Перенести дату выпуска</a>';
  $output_a .= '<a class="btn btn-primary autodialog" href="' . $url . '?subject=volume">Изменить объём выпуска</a>';
  $output_a .= '<a class="btn btn-default autodialog" href="' . $url . '?subject=cancel">Отменить выпуск</a>';
  $output_a .= '<a class="btn btn-default autodialog" href="' . $url . '?subject=docs">Отправить документы</a>';

  // сформировать вывод
  $output .=  '<div class="nom-page">';
  $output .=    '<div class="n-header">';
  $output .=      '<div class="n-title">';
  $output .=        $title;
  $output .=      '</div>';
  $output .=      '<div class="n-volume">';
  $output .=        $volume;
  $output .=      '</div>';
  $output .=      '<div class="n-date">';
  $output .=        $date_start;
  $output .=      '</div>';
  $output .=    '</div>';

  $output .=    '<div class="n-plan">';
  $output .=      $output_pu;
  $output .=    '</div>';

  $output .=    '<div class="n-history">';
  $output .=      '<div class="n-actions">';
  $output .=      '<label class="label">Действия</label>';
  $output .=        $output_a;
  $output .=      '</div>';

  $output .=      '<div class="n-messages">';
  $output .=      '<label class="label">История изменений</label>';
  $output .=        $output_m;
  $output .=      '</div>';
  $output .=    '</div>';
  $output .=  '</div>';

  return $output;
}

function prod_message_add_form($form, $form_state, $nomenklatura_id = null, $month_start = null)
{
  // тема Сообщения
  if (!$nomenklatura_id) $subject = 'add';
  else $subject = $_GET['subject'] ?? '';

  $intro = '';

  $form = [
    '#prefix' => '<div class="message-add-wrapper">',
    '#suffix' => '</div>',
    'subject' => ['#type' => 'hidden', '#value' => $subject],
  ];

  // если не задана Номенклатура, то это Сообщение о новом продукте
  if (!$nomenklatura_id) {
    $intro = '<p>Чтобы добавить заявку на выпуск продукта, выберите номенклатуру, задайте месяц и объём производства.</p>';

    // подготовить список Номенклатуры клиента (если несколько компаний, то последовательно)
    $options = [];
    if ($companies = ext_user_get_companies_by_user()) {
      foreach ($companies as $company) {
        $nom_list = prod_nomenklatura_get_list(['company_ids' => [$company['id']]]);
        foreach ($nom_list as $nom) {
          $nom_info = production_nomenklatura_get_info($nom->nom_id);
          $options[$nom->nom_id] = $nom_info['label'];
        }
      }
    }
  } else {
    $nom_info = production_nomenklatura_get_info($nomenklatura_id);
    $options[$nomenklatura_id] = $nom_info['label'];

    if ($plan_rec = prod_plan_record_get_last($month_start, $nomenklatura_id)) {
      $plan_rec_info = prod_plan_record_get_info($plan_rec->id);
    }
  }

  if ($subject == 'date') $intro = '<p>Для переноса даты производства напишите соответствующий комментарий в сообщении.</p>';
  if ($subject == 'volume') $intro = '<p>Для изменения объёма выпуска заполните соответствующее поле и при необходимости напишите комментарий.</p>';
  if ($subject == 'cancel') $intro = '<p>Для отмены выпуска продукта просто нажмите "Отправить сообщение". При необходимости заполните поле комментария.</p>';


  $form['intro'] = [
    '#markup' => '<div class="intro">' . $intro . '</div>',
  ];


  $form['nom'] = [
    '#title' => 'Номенклатура',
    '#type' => 'select',
    '#options' => $options,
    '#disabled' => $subject != 'add',
    '#default_value' => $nomenklatura_id ?? array_key_first($options),
  ];

  $month = empty($form_values["values"]["month"]) ? date('n') : $form_values["values"]['month'];
  $year = empty($form_values["values"]['year']) ? date('Y') : $form_values["values"]['year'];

  $month_opts = [];
  for($i = 1; $i <= 12; $i++) {
    $month_opts[$i] = helper_month_label_ru($i);
  }
  $form['month']['month'] = [
    '#title' => 'Месяц',
    '#type' => 'select',
    '#options' => $month_opts,
    '#default_value' => $month,
    '#disabled' => $subject != 'add',
    '#ajax' => [
      'callback' => 'prod_message_add_callback',
      'wrapper' => 'nom-list-form-wrapper',
    ],
    '#prefix' => '<div class="row"><div class="col-xs-12 col-md-8"><div class="month-select-wrapper">',
  ];
  $year_opts = [];
  for($i = 0; $i <= 2; $i++) {
    $yr = date('Y') + $i;
    $year_opts[$yr] = $yr;
  }
  $form['month']['year'] = [
    '#title' => 'Год',
    '#type' => 'select',
    '#options' => $year_opts,
    '#default_value' => $year,
    '#disabled' => $subject != 'add',
    '#ajax' => [
      'callback' => 'prod_message_add_callback',
      'wrapper' => 'nom-list-form-wrapper',
    ],
    '#suffix' => '</div></div>',
  ];

  $volume_curr = $plan_rec_info['volume'] ?? 0;

  $form['volume'] = [
    '#type' => 'textfield',
    '#size' => 20,
    '#title' => 'Объём, л(кг)',
    '#default_value' => $subject == 'cancel' ? 0 : $volume_curr,
    '#disabled' => !in_array($subject, ['add', 'volume']),
    '#prefix' => '<div class="col-xs-12 col-md-4">',
    '#suffix' => '</div></div>',
  ];

  $form['comment'] = [
    '#type' => 'textarea',
    '#rows' => 5,
    '#resizable' => false,
    '#title' => 'Комментарий к заявке',
  ];

  $form['actions'] = ['#type' => 'actions'];
  $form['actions']['submit'] = [
    '#type' => 'submit',
    '#value' => 'Отправить сообщение',
  ];


  return $form;
}

function prod_message_add_form_submit($form, &$form_state)
{
  $month_start = gmmktime(0, 0, 0, $form_state['values']['month'], 1 , $form_state['values']['year']);
  $nom_id = $form_state['values']['nom'];

  //
  $change = null;
  $volume = $form_state['values']['volume'];
  $comment = $form_state['values']['comment'];

  // сформировать Изменение
  $subject = $form_state['values']['subject'];
  if ($subject == 'add')    $change = 4738;
  if ($subject == 'date')   $change = 4739;
  if ($subject == 'volume') $change = 4740;
  if ($subject == 'cancel') $change = 4741;

  // сформировать комментарий, так как, например, объём в сообщении не сохраняется
  if (in_array($subject, ['add', 'volume'])) {
    $comment .= ' ' . '(заданный объём: ' . helper_number_format($volume, 2, ' ') . ' л(кг))';
  }

  // todo добавить сообщение
  prod_message_create([
    'created' => REQUEST_TIME,
    'month_ts' => $month_start,
    'nom_id' => $nom_id,
    'author' => PROD_MESSAGE_AUTHOR_CLIENT,
    'comment' => $comment,
    'changes' => $change ? [$change] : [],
  ]);

  // todo экспортировать Сообщение


  // редирект на страницу Номенклатуры
  $form_state['redirect'] = '/production/nomenklatura/' . $form_state['values']['nom'] . '/' . $month_start;
}
