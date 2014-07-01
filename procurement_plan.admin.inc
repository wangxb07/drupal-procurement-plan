<?php

/**
 * Generates the procurement plan type editing form.
 */
function procurement_plan_type_form($form, &$form_state, $plan_type, $op = 'edit') {
  if ($op == 'clone') {
    $plan_type->label .= ' (cloned)';
    $plan_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $plan_type->label,
    '#description' => t('The human-readable name of this plan type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($plan_type->type) ? $plan_type->type : '',
    '#maxlength' => 32,
    '#disabled' => $plan_type->isLocked() && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'procurement_plan_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this plan type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#default_value' => isset($plan_type->description) ? $plan_type->description : '',
    '#description' => t('Description about the plan type.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save plan type'),
    '#weight' => 40,
  );

  if (!$plan_type->isLocked() && $op != 'add' && $op != 'clone') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete plan type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('procurement_plan_type_form_submit_delete')
    );
  }
  return $form;
}

/**
 * validation handler for creating/editing plan_type.
 */
function procurement_plan_type_form_validate(&$form, &$form_state) {
  if (is_numeric($form_state['values']['type'])) {
    form_error($form['type'], t('machine name not allow numerice'));
  }
}

/**
 * Submit handler for creating/editing plan_type.
 */
function procurement_plan_type_form_submit(&$form, &$form_state) {
  $plan_type = entity_ui_form_submit_build_entity($form, $form_state);
  // Save and go back.
  procurement_plan_type_save($plan_type);

  // Redirect user back to list of plan types.
  $form_state['redirect'] = 'admin/structure/procurement-plan-types';
}

function procurement_plan_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/procurement-plan-types/manage/' . $form_state['procurement_plan_type']->type . '/delete';
}

/**
 * Page to select plan Type to add new plan.
 */
function procurement_plan_admin_add_page() {
  $items = array();

  $params = drupal_get_query_parameters();

  foreach (procurement_plan_types() as $plan_type_key => $plan_type) {
    $items[] = l(entity_label('procurement_plan_type', $plan_type), 'procurement-plan/add/' . $plan_type_key, array(
                 'query' => $params
               ));
  }
  return array('list' => array('#theme' => 'item_list', '#items' => $items, '#title' => t('Select type of plan to create.')));
}

/**
 * Add new plan page callback.
 */
function procurement_plan_add($type) {
  $plan_type = procurement_plan_types($type);

  $plan = entity_create('procurement_plan', array('type' => $type, 'status' => 'init'));
  drupal_set_title(t('Create @name', array('@name' => entity_label('procurement_plan_type', $plan_type))));

  $output = drupal_get_form('procurement_plan_form', $plan);

  return $output;
}

/**
 * Plan Form.
 */
function procurement_plan_form($form, &$form_state, $plan) {
  $params = drupal_get_query_parameters();
  $form_state['plan'] = $plan;

  // Ensure the owner name is accessible if the uid is set.
  if (!empty($plan->uid) && $owner = user_load($plan->uid)) {
    $plan->name = $owner->name;

    if (empty($plan->mail)) {
      $plan->mail = $owner->mail;
    }
  }

  if (empty($plan->created)) {
    $plan->date = format_date(REQUEST_TIME, 'custom', 'Y-m-d H:i:s O');
  }

  $form['submission'] = array(
    '#type' => 'fieldset',
    '#title' => t('Submission'),
    '#weight' => -10,
  );

  $sid = 0;
  $nid = 0;

  if (!empty($params['nid']) && !empty($params['sid'])) {
    $nid = $params['nid'];
    $sid = $params['sid'];
  }

  if (!empty($plan->nid) && !empty($plan->sid)) {
    $nid = $plan->nid;
    $sid = $plan->sid;
  }

  if (!empty($sid) && !empty($nid)) {
    // load submission info and related node info
    // get submission output
    $node = node_load($nid);

    $submission = webform_menu_submission_load($sid, $nid);
    $output = webform_submission_render($node, $submission, NULL, 'full');

    $form['submission']['info'] = array(
      '#type' => 'markup',
      '#markup' => $output,
    );

    $form['sid'] = array(
      '#type' => 'hidden',
      '#value' => $sid,
    );
    $form['nid'] = array(
      '#type' => 'hidden',
      '#value' => $nid,
    );

    // fill the plan name
    if (empty($plan->name) && $submission_owner = user_load($submission->uid)) {
      $plan->name = $submission_owner->name;

      if (empty($plan->mail)) {
        $plan->mail = $submission_owner->mail;
      }
    }
  }
  else {
    $query = db_select('webform', 'w');
    $query->join('node', 'n', 'w.nid = n.nid');
    $query->fields('n');
    $webforms = $query->execute()->fetchAllAssoc('nid');

    $webform_opions = array(
      '0' => 'None',
    );
    foreach ($webforms as $webform) {
      $webform_opions[$webform->nid] = $webform->title;
    }

    $form['submission']['webform'] = array(
      '#type' => 'select',
      '#title' => t('Webforms'),
      '#description' => t('Choose the related webform.'),
      '#options' => $webform_opions,
      '#default_value' => !empty($plan->nid) ? $plan->nid : '', // create plan
      '#weight' => -1,
      '#ajax' => array(
        'callback' => 'submission_webforms_select_callback',
        'wrapper' => 'submission-select',
      ),
    );

    $form['submission']['container'] = array(
      '#type' => 'container',
      '#title' => t("Submission select"),
      '#prefix' => '<div id="submission-select">',
      '#suffix' => '</div>',
    );

    // under code for ajax

    // load webform inc file
    module_load_include('inc', 'webform', 'includes/webform.submissions');
    if (isset($form_state['values']['webform'])) {
      $webform_nid = $form_state['values']['webform'];

      $submissions = webform_get_submissions($webform_nid);
      if (!empty($submissions)) {
        $submission_options = array();
        foreach ($submissions as $submission) {

          $submission_infos = array(
            $submission->is_draft ? t('#@sid (draft)', array('@sid' => $submission->sid)) : '#' . $submission->sid,
            format_date($submission->submitted, 'short'),
            theme('username', array('account' => $submission)));

          $submission_options[$submission->sid] = '[' . implode('][  ', $submission_infos) . ']';
        }
        $form['submission']['container']['sid'] = array(
          '#type' => 'checkboxes',
          '#description' => t('Choose the related webform submission'),
          '#options' => $submission_options,
        );
      }
      else {
        $form['submission']['container']['sid'] = array(
          '#type' => 'markup',
          '#markup' => t('Nothing submitted for this webform'),
        );
      }
    }
  }

  $form['title'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Name'),
    '#default_value' => $plan->title,
  );

  $form['plan_number'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Plan Number'),
    '#default_value' => $plan->plan_number,
  );

  field_attach_form('procurement_plan', $plan, $form, $form_state);

  $form['additional_settings'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 99,
  );

  $form['plan_status'] = array(
    '#type' => 'fieldset',
    '#title' => t('Plan status'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#group' => 'additional_settings',
    '#attached' => array(
      'js' => array(
        drupal_get_path('module', 'procurement_plan') . '/procurement_plan.js',
        array(
          'type' => 'setting',
          'data' => array('status_titles' => procurement_plan_status_get_title()),
        ),
      ),
    ),
    '#weight' => 20,
  );

  // Build an array of plan status options grouped by order state.
  $options = procurement_plan_status_options_list();

  $form['plan_status']['status'] = array(
    '#type' => 'select',
    '#title' => t('Status'),
    '#options' => $options,
    '#default_value' => $plan->status,
  );
  $form['plan_status']['status_original'] = array(
    '#type' => 'hidden',
    '#value' => $plan->status,
    '#attributes' => array('id' => 'edit-status-original'),
  );
  $form['plan_status']['log'] = array(
    '#type' => 'textarea',
    '#title' => t('Update log message'),
    '#description' => t('Provide an explanation of the changes you are making. This will provide a meaningful audit trail for updates to this plan.'),
    '#rows' => 4,
  );

  $form['user'] = array(
    '#type' => 'fieldset',
    '#title' => t('User information'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#access' => user_access('administer procurement_plan entities'),
    '#group' => 'additional_settings',
    '#attached' => array(
      'js' => array(
        drupal_get_path('module', 'procurement_plan') . '/procurement_plan.js',
        array(
          'type' => 'setting',
          'data' => array('anonymous' => variable_get('anonymous', t('Anonymous'))),
        ),
      ),
    ),
    '#weight' => 30,
  );
  $form['user']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Owned by'),
    '#description' => t('Leave blank for %anonymous.', array('%anonymous' => variable_get('anonymous', t('Anonymous')))),
    '#maxlength' => 60,
    '#autocomplete_path' => 'user/autocomplete',
    '#default_value' => !empty($plan->name) ? $plan->name : '', // create plan
                                                                // with customer name
    '#weight' => -1,
  );
  $form['user']['mail'] = array(
    '#type' => 'textfield',
    '#title' => t('Plan contact e-mail'),
    '#description' => t('Defaults to the owner account e-mail address if left blank. Used for plan communication.'),
    '#default_value' => isset($plan->mail) ? $plan->mail: '',
  );

  $form['plan_history'] = array(
    '#type' => 'fieldset',
    '#title' => t('Plan history', array(), array('context' => 'a drupal procurement plan')),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#group' => 'additional_settings',
    '#attached' => array(
      'js' => array(drupal_get_path('module', 'procurement_plan') . '/procurement_plan.js'),
    ),
    '#weight' => 40,
  );

  $form['plan_history']['revision'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create new revision on update'),
    '#description' => t('If an update log message is entered, a revision will be created even if this is unchecked.'),
    '#default_value' => variable_get('procurement_plan_auto_revision', TRUE),
    '#access' => user_access('administer procurement_plan entities'),
  );
  $form['plan_history']['date'] = array(
    '#type' => 'textfield',
    '#title' => t('Created on'),
    '#description' => t('Format: %time. The date format is YYYY-MM-DD and %timezone is the time zone offset from UTC. Leave blank to use the time of form submission.', array('%time' => !empty($plan->date) ? date_format(date_create($plan->date), 'Y-m-d H:i:s O') : format_date($plan->created, 'custom', 'Y-m-d H:i:s O'), '%timezone' => !empty($plan->date) ? date_format(date_create($plan->date), 'O') : format_date($plan->created, 'custom', 'O'))),
    '#maxlength' => 25,
    '#default_value' => !empty($plan->created) ? format_date($plan->created, 'custom', 'Y-m-d H:i:s O') : '',
  );
  $form['plan_history']['created'] = array(
    '#type' => 'hidden',
    '#value' => !empty($plan->created) ? format_date($plan->created, 'short') : '',
    '#attributes' => array('id' => 'edit-created'),
  );
  $form['plan_history']['changed'] = array(
    '#type' => 'hidden',
    '#value' => !empty($plan->changed) ? format_date($plan->changed, 'short') : '',
    '#attributes' => array('id' => 'edit-changed'),
  );


  $submit = array();
  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions'] = array(
    '#weight' => 100,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save plan'),
    '#submit' => $submit + array('procurement_plan_form_submit'),
  );

  // Show Delete button if we edit plan.
  $plan_id = entity_id('procurement_plan' ,$plan);
  if (!empty($plan_id) && procurement_plan_access('edit', $plan)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('procurement_plan_form_submit_delete'),
    );
  }

  $form['#validate'][] = 'procurement_plan_form_validate';

  return $form;
}

/**
 * Ajax callback
 */
function submission_webforms_select_callback($form, $form_state) {
  return $form['submission']['container'];
}

/**
 * Procurement plan form validate handler.
 */
function procurement_plan_form_validate($form, &$form_state) {
  // if sid is null, nid set to null too.
  if (!empty($form_state['values']['nid']) && empty($form_state['values']['sid'])) {
    $form_state['values']['nid'] = 0;
  }
}

/**
 * Plan submit handler.
 */
function procurement_plan_form_submit($form, &$form_state) {
  $plan = $form_state['plan'];

  if ($form_state['values']['revision'] || !empty($form_state['values']['log'])) {
    $plan->revision = TRUE;
    $plan->log = $form_state['values']['log'];
  }

  if (!empty($form_state['values']['name']) && $account = user_load_by_name($form_state['values']['name'])) {
    // If the plan is being converted to an authenticated plan from an
    // anonymous plan...
    if ($plan->uid == 0) {
      // Set the converted boolean for later processing.
      $converted = TRUE;
    }

    $plan->uid = $account->uid;

    if (empty($form_state['values']['mail'])) {
      $plan->mail = $account->mail;
    }
  }
  else {
    $plan->uid = 0;
  }

  if (!empty($form_state['values']['mail'])) {
    $plan->mail = $form_state['values']['mail'];
  }

  // Update the order status if specified.
  if ($form_state['values']['status'] != $form_state['values']['status_original']) {
    // We skip order saving in the update since we do it below once for the
    // entire form submission.
    procurement_plan_status_update($plan, $form_state['values']['status'], TRUE);
  }

  $plan->created = !empty($form_state['values']['date']) ? strtotime($form_state['values']['date']) : REQUEST_TIME;

  entity_form_submit_build_entity('procurement_plan', $plan, $form, $form_state);

  procurement_plan_save($plan);

  $plan_uri = entity_uri('procurement_plan', $plan);

  $form_state['redirect'] = $plan_uri['path'];

  drupal_set_message(t('Plan %title saved.', array('%title' => entity_label('procurement_plan', $plan))));
}

function procurement_plan_form_submit_delete($form, &$form_state) {
  $plan = $form_state['plan'];
  $plan_uri = entity_uri('procurement_plan', $plan);
  $form_state['redirect'] = $plan_uri['path'] . '/delete';
}

/**
 * Delete confirmation form.
 */
function procurement_plan_delete_form($form, &$form_state, $plan) {
  $form_state['plan'] = $plan;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['plan_type_id'] = array('#type' => 'value', '#value' => entity_id('procurement_plan' ,$plan));
  $plan_uri = entity_uri('procurement_plan', $plan);
  return confirm_form($form,
    t('Are you sure you want to delete plan %title?', array('%title' => entity_label('procurement_plan', $plan))),
    $plan_uri['path'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Delete form submit handler.
 */
function procurement_plan_delete_form_submit($form, &$form_state) {
  $plan = $form_state['plan'];
  procurement_plan_delete($plan);

  drupal_set_message(t('Plan %title deleted.', array('%title' => entity_label('procurement_plan', $plan))));

  $form_state['redirect'] = '<front>';
}


/**
 * Form constructor for the plan status toggle confirmation form.
 *
 * @see procurement_plan_status_toggle_confirm_submit()
 */
function procurement_plan_status_toggle_confirm($form, &$form_state, $plan) {
  $form['#plan'] = $plan;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['ppid'] = array('#type' => 'value', '#value' => $plan->ppid);
  return confirm_form($form,
    t('Are you sure you want to toggle status %title?', array('%title' => $plan->title)),
    'node/' . $plan->ppid,
    t('This action cannot be undone.'),
    t('Toggle'),
    t('Cancel')
  );
}

/**
 * Executes procurement plan deletion.
 *
 * @see procurement_plan_status_toggle_confirm()
 */
function procurement_plan_status_toggle_confirm_submit($form, &$form_state) {
  rules_invoke_event('procurement_plan_status_toggle', $form['#plan']);
  $form_state['redirect'] = 'admin/purchase-plan/all';
}
