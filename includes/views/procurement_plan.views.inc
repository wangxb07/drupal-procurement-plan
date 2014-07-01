<?php

/**
 * Export Drupal Procurement plan to Views.
 */

/**
 * Impelements hook_views_data_alter(&$data)
 */
function procurement_plan_views_data_alter(&$data) {
  // Expose the product type.
  $data['procurement_plan']['plan_type'] = array(
    'title' => t('Type'),
    'help' => t('The human-readable name of the type of the procurement plan.'),
    'field' => array(
      'handler' => 'procurement_plan_handler_field_plan_type',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'procurement_plan_handler_filter_plan_type',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['procurement_plan']['created'] = array(
    'title' => t('Post date'), // The item it appears as on the UI,
    'help' => t('The date of plan was posted.'), // The help that appears on the UI,
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
  );
  $data['procurement_plan']['changed'] = array(
    'title' => t('Last update date'), // The item it appears as on the UI,
    'help' => t('The date of plan last update.'), // The help that appears on the UI,
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
  );

  $data['procurement_plan']['status']['field'] = array(
    'handler' => 'procurement_plan_handler_field_plan_status',
  );

}
