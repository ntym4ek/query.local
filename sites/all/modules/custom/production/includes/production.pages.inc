<?php

define ('MONTHS', [ 1 => 'Январь' , 'Февраль' , 'Март' , 'Апрель' , 'Май' , 'Июнь' , 'Июль' , 'Август' , 'Сентябрь' , 'Октябрь' , 'Ноябрь' , 'Декабрь' ]);


function production_nom_list_form($form, $form_values)
{
  $month = empty($form_values["values"]["month"]) ? date('n') : $form_values["values"]['month'];
  // todo исправить год по умолчанию
  $year = empty($form_values["values"]['year']) ? date('Y')-1 : $form_values["values"]['year'];

  $form = [
    '#prefix' => '<div id="nom-list-form-wrapper" class="nomenklatura">',
    '#suffix' => '</div>',
    'filters' => [
      '#type' => 'container',
      '#attributes' => ['class' => ['filters']],
    ]
  ];

  $month_opts = [];
  for($i = 1; $i <= 12; $i++) {
    $month_opts[$i] = MONTHS[$i];
  }
  $form['filters']['month'] = [
    '#title' => 'Месяц',
    '#type' => 'select',
    '#options' => $month_opts,
    '#default_value' => [$month],
    '#ajax' => [
      'callback' => 'production_nom_list_form_callback',
      'wrapper' => 'queries-list-form-wrapper',
    ],
    '#prefix' => '<div class="date-wrapper filter">',
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
      'callback' => 'production_nom_list_form_callback',
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

  $company_default = array_key_first($options);

  $form['filters']['company'] = [
    '#title' => 'Компания',
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => $company_default,
    '#required' => true,
    '#ajax' => [
      'callback' => 'production_nom_list_form_callback',
      'wrapper' => 'nom-list-form-wrapper',
    ],
    '#prefix' => '<div class="filter">',
    '#suffix' => '</div>',
  ];


  // todo получить список актуальных заявок по номенклатуре
  $noms_arr = [];

  // получить список номенклатуры
  $month_start = gmmktime(0, 0, 0, $month, 1, $year);
  $nom_list = production_nomenklatura_get_list(['company_id' => $company_default, 'month_ts' => $month_start]);
  foreach($nom_list as $nom_id => $nom) {
    $noms_arr[] = production_nomenklatura_get_info($nom_id);
  }


  // todo вывести список
  $output = '<div class="nom-list">';
  foreach ($noms_arr as $nom_id => $nom_item) {
    $output .= theme('nomenkaltura_teaser', ['nom_info' => $nom_item]);
  }
  $output .= '</div>';

  $form['list'] = [
    '#markup' => $output,
  ];

  return $form;
}
function production_nom_list_form_callback($form, $form_values)
{
  return $form;
}

function production_pu_load_form($form, $form_values)
{
  $month = empty($form_values["values"]["month"]) ? date('n') : $form_values["values"]['month'];
  $year = empty($form_values["values"]['year']) ? date('Y') : $form_values["values"]['year'];

  $form = [
    '#prefix' => '<div id="pu-load-form-wrapper">',
    '#suffix' => '</div>',
  ];

  $month_opts = [];
  for($i = 1; $i <= 12; $i++) {
    $month_opts[$i] = MONTHS[$i];
  }
  $form['month'] = [
    '#title' => 'Месяц',
    '#type' => 'select',
    '#options' => $month_opts,
    '#default_value' => [$month],
    '#ajax' => [
      'callback' => 'production_pu_load_form_callback',
      'wrapper' => 'pu-load-form-wrapper',
    ],
    '#prefix' => '<div class="date-wrapper">',
  ];
  $year_opts = [];
  for($i = -1; $i <= 1; $i++) {
    $yr = date('Y') + $i;
    $year_opts[$yr] = $yr;
  }
  $form['year'] = [
    '#title' => '',
    '#type' => 'select',
    '#options' => $year_opts,
    '#default_value' => [$year],
    '#ajax' => [
      'callback' => 'production_pu_load_form_callback',
      'wrapper' => 'pu-load-form-wrapper',
    ],
    '#suffix' => '</div>',
  ];

  $month_start = gmmktime(0, 0, 0, $month, 1, $year);
  $produce_units = production_produce_unit_get_load($month_start);
  $output =
    '</div>' .
      '<div class="produce-unit-legenda top"><div><span></span>- установка свободна</div><div><span class="loaded"></span>- установка занята</div><div><span class="company-loaded"></span>- продукция компании</div>' .
    '</div>';

  // вывести занятость по каждой установке
  $output .=
    '<div class="produce-units">' .
      '<div class="produce-units-list">';
  foreach ($produce_units as $pu_tid => $pu_item) {
    $output .= theme('produce_unit_load', ['produce_unit' => $pu_item, 'month_start' => $month_start, 'show_amount' => true,  'show_nomenklatura' => true]);
  }
  $output .=
      '</div>' .
    '</div>';

  $form['list'] = [
    '#markup' => $output,
  ];

  return $form;
}
function production_pu_load_form_callback($form, $form_values)
{
  return $form;
}

function production_year_page()
{
  $html = '';

  $filter_form = drupal_get_form('production_year_client_form');
  $html .= drupal_render($filter_form);
  $client_id = empty($_REQUEST['client']) ? $filter_form['client']['#default_value'] : $_REQUEST['client'];

  $html .= production_year_table($client_id);

  return $html;
}

function production_year_client_form($form, $form_state)
{
  $options = [];
  $year = date('n') < 9 ? date('Y') : date('Y')+1;
  $month_start = gmmktime(0, 0, 0, 9, 1, $year-1);


  $query = db_select('taxonomy_term_data', 't');
  $query ->fields('t', ['tid', 'name']);
  $query ->orderBy('name');
  $query ->innerJoin('field_data_field_client', 'fc', 'fc.field_client_tid = t.tid');
  // клиенту ограничиваем выбор его компаниями
  if (user_has_role(ROLE_USER_PRODUCTION)) {
    $companies = ext_user_get_companies_by_user();
    $query ->condition('fc.field_client_tid', array_keys($companies),'IN');
  }
  $query ->condition('fc.bundle', 'nomenklatura');
  $query ->innerJoin('field_data_field_nomenklatura', 'fn', 'fn.field_nomenklatura_target_id = fc.entity_id');
  $query ->innerJoin('eck_query', 'q', 'q.id = fn.entity_id');
  $query ->condition('q.created', $month_start, '>=');
  if ($clients = $query->execute()->fetchAllAssoc('tid')) {
    foreach ($clients as $tid => $client) {
      $options[$tid] = $client->name;
    }
  }

  $form['client'] = array(
    '#title' => 'Компания',
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => array_key_first($options),
    '#required' => true,
    '#ajax' => [
      'callback' => 'production_year_select_callback',
      'wrapper' => 'table-wrapper',
      'method' => 'replace',
      'effect' => 'fade',
    ],
    '#prefix' => '<div class="row"><div class="col-xs-4">',
    '#suffix' => '</div></div>',
  );

  return $form;
}
function production_year_client_form_submit($form, &$form_state)
{
  $form_state['redirect'] = FALSE;
}
function production_year_select_callback($form, $form_state)
{
  return production_year_table($form_state['values']['client']);
}

function production_year_table($client_id)
{
  $year = date('n') < 9 ? date('Y') : date('Y')+1;

  $nom_arr = [
    '%nom_id' => [
      'months' => [
        '%month' => '%amount',
      ],
    ],
  ];
  $nom_arr = [];
  // получить список номенклатуры
  $nom_id_list = production_nomenklatura_get_list_by_client_id($client_id);
  foreach($nom_id_list as $index => $nom_id) {
    $nom_info = production_nomenklatura_get_info($nom_id);
    $nom_item = [
      'year' => ['plan' => 0, 'fact' => 0],
      'months' => [],
      'label' => $nom_info['label'],
    ];
    // последние актуальные заявки по Номенклатуре за 12 месяцев начиная с сентября
    for ($i = 1; $i <= 12; $i++) {
      $month_start = gmmktime(0, 0, 0, 9+$i-1, 1, $year-1);
      $month_num = date('n', $month_start);
      $nom_item['months'][$month_num] = 0;
      if ($query_actual = production_query_get_actual($month_start, $nom_id)) {
        $query_info = production_query_get_info($query_actual);
        $nom_item['months'][$month_num] = $query_info['output'];
        $nom_item['year']['plan'] += $query_info['output']['plan'];
        $nom_item['year']['fact'] += $query_info['output']['fact'];
      }
    }
    if ($nom_item['year']['plan']) {
      $nom_arr[$nom_id] = $nom_item;
    }
//    if (count($nom_arr) > 10) break;
  }

  // шапка
  $header = ['Наименование', ['data' => 'План', 'align' => 'center'], ['data' => 'Факт', 'align' => 'center']];
  foreach ([9,10,11,12,1,2,3,4,5,6,7,8] as $m_num) {
    $header[] = ['data' => MONTHS[$m_num], 'align' => 'center'];
  }

  // табличная часть
  $rows = [];
  foreach ($nom_arr as $n_tid => $n_item) {
    $row = [$n_item['label'], ['data' => number_format($n_item['year']['plan'], 1, ',', ' '), 'align' => 'right'], ['data' => number_format($n_item['year']['fact'], 1, ',', ' '), 'align' => 'right']];
    foreach ([9,10,11,12,1,2,3,4,5,6,7,8] as $m_num) {
      $month_start = gmmktime(0, 0, 0, $m_num, 1, $m_num < 9 ? $year : $year-1);
      $class = 'green'; $align = 'right';
      if ($month_start > REQUEST_TIME) $class = 'red';
      if (date('n', $month_start) == date('n')) {
        $class = ''; $align = 'center';
        $output = $n_item['months'][$m_num]['plan'] ? number_format($n_item['months'][$m_num]['plan'], 1, ',', ' ') . ' / ' . number_format($n_item['months'][$m_num]['fact'], 1, ',', ' ') : '';
      } else {
        $output = $n_item['months'][$m_num]['plan'] ? number_format($n_item['months'][$m_num]['plan'], 1, ',', ' ') : '';
      }


      $row[] = ['data' => $output, 'align' => $align, 'class' => $class];
    }
    $rows[] = $row;
  }

  return  '<div id="table-wrapper">' .
            '<h5>Таблица носит ознакомительный характер и может содержать неактуальные данные. За точной информацией обратитесь к нашему менеджеру.</h5>' .
            theme('table', ['header' => $header, 'rows' => $rows, 'attributes' => ['class' => ['table-year-plan']]]) .
          '</div>';
}

