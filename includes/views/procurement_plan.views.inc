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

  $data['procurement_plan']['view_procurement_plan']['moved to'] = array('views_entity_procurement_plan', 'view_procurement_plan');
  $data['views_entity_procurement_plan']['view_procurement_plan'] = array(
    'field' => array(
      'title' => t('Link'),
      'help' => t('Provide a simple link to the content.'),
      'handler' => 'procurement_plan_handler_field_plan_link',
    ),
  );

  $data['procurement_plan']['edit_procurement_plan']['moved to'] = array('views_entity_procurement_plan', 'edit_procurement_plan');
  $data['views_entity_procurement_plan']['edit_procurement_plan'] = array(
    'field' => array(
      'title' => t('Edit link'),
      'help' => t('Provide a simple link to edit the content.'),
      'handler' => 'procurement_plan_handler_field_plan_link_edit',
    ),
  );

  $data['procurement_plan']['delete_procurement_plan']['moved to'] = array('views_entity_procurement_plan', 'delete_procurement_plan');
  $data['views_entity_procurement_plan']['delete_procurement_plan'] = array(
    'field' => array(
      'title' => t('Delete link'),
      'help' => t('Provide a simple link to delete the content.'),
      'handler' => 'procurement_plan_handler_field_plan_link_delete',
    ),
  );

  $data['procurement_plan']['uid'] = array(
    'title' => t('Author uid'),
    'help' => t('The user authoring the content. If you need more fields than the uid add the content: author relationship'),
    'relationship' => array(
      'title' => t('Author'),
      'help' => t('Relate content to the user who created it.'),
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'field' => 'uid',
      'label' => t('author'),
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_user_name',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'field' => array(
      'handler' => 'views_handler_field_user',
    ),
  );

}
