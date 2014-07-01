<?php

/**
 * @file
 * Rules integration for procurement plan
 *
 * @addtogroup rules
 */

/**
 * Implements hook_rules_event_info().
 */
function procurement_plan_rules_event_info() {
  $events = array();

  $events['procurement_plan_status_toggle'] = array(
    'label' => t('After plan status toggle confirmed'),
    'group' => t('Procurement Plan'),
    'variables' => array(
      'procurement_plan' => array(
        'type' => 'procurement_plan',
        'label' => 'Procurement Plan',
      ),
    ),
    'access callback' => 'procurement_plan_rules_access',
  );

  return $events;
}

/**
 * Implements hook_rules_condition_info().
 */
function procurement_plan_rules_condition_info() {
  $conditions = array();

  $conditions['procurement_plan_compare_status'] = array(
    'label' => t('Procurement plan compare status'),
    'parameter' => array(
      'procurement_plan' => array(
        'type' => 'procurement_plan',
        'label' => t('Procurement Plan'),
      ),
      'status' => array(
        'type' => 'text',
        'label' => t('Status'),
        'options list' => 'procurement_plan_status_options_list',
        'description' => t('The status to compare.'),
        'restriction' => 'input',
      ),
    ),
    'group' => t('Procurement Plan'),
    'callbacks' => array(
      'execute' => 'procurement_plan_rules_status_compare',
    ),
  );

  return $conditions;
}

/**
 * Implements hook_rules_action_info().
 */
function procurement_plan_rules_action_info() {
  $actions = array();

  $actions['procurement_plan_status_toggle'] = array(
    'label' => t('toggle procurement plan status'),
    'parameter' => array(
      'procurement_plan' => array(
        'type' => 'procurement_plan',
        'label' => t('Procurement plan to toggle status'),
      ),
      'status' => array(
        'type' => 'text',
        'label' => t('Status'),
        'options list' => 'procurement_plan_status_options_list',
        'description' => t('The status to compare.'),
        'restriction' => 'input',
      ),
    ),
    'group' => t('Procurement Plan'),
    'callbacks' => array(
      'execute' => 'procurement_plan_rules_status_toggle',
    ),
  );

  return $actions;
}

/**
 * compare status callback
 *
 * @param procurement_plan $plan
 * @param string $status
 *
 * @return boolean
 */
function procurement_plan_rules_status_compare($plan, $status) {
  return $plan->status == $status;
}

/**
 * toggle status callback
 *
 * @param procurement_plan $plan
 * @param string $status
 *
 * @return void
 */
function procurement_plan_rules_status_toggle($plan, $status) {
  $plan->status = $status;
  procurement_plan_save($plan);
}
