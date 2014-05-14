<?php

/**
 * Export Drupal Procurement plan to Views.
 */

/**
 * Implements hook_views_data()
 */
function procurement_plan_views_data() {
  $data = array();

  $data['procurement_plan']['table']['group']  = t('Procurement Plan');

  $data['procurement_plan']['table']['base'] = array(
    'field' => 'ppid',
    'title' => t('Procurement Plan'),
    'help' => t('business procument plan'),
    'access query tag' => 'procurement_plan_access',
  );

  $data['procurement_plan']['table']['entity type'] = 'procurement_plan';

  // Expose primary key
  $data['procurement_plan']['ppid'] = array(
    'title' => t('Procurement Plan ID'),
    'help' => t('The unique internal identifier of the procurement plan.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'procurement_plan_handler_argument_procurement_plan_ppid',
      'name field' => 'title',
      'numeric' => TRUE,
      'validate type' => 'ppid',
    ),
  );

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

  // Expose the title.
  $data['procurement_plan']['title'] = array(
    'field' => array(
      'title' => t('Title'),
      'help' => t('The title of the procurement title.'),
      'handler' => 'procurement_plan_handler_field_title',
    ),
  );

  return $data;
}
