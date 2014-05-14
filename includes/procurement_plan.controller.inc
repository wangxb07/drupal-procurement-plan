<?php

/**
 * @file
 * The controller for the procurement plan entity containing the CRUD operations.
 */

/**
 * The controller class for procurement plans contains methods for the procurement plan CRUD
 * operations. The load method is inherited from the default controller.
 */
class ProcurementPlanController extends EntityAPIController {

  /**
   * Create a default procurement plan.
   *
   * @param array $values
   *   An array of values to set, keyed by property name.
   *
   * @return
   *   A procurement plan object with all default fields initialized.
   */
  public function create(array $values = array()) {
    $values += array(
      'ppid' => NULL,
      'sid' => 0,
      'nid' => 0,
      'title' => '',
      'uid' => '',
      'mail' => ( !empty($values['uid']) && ($account = user_load($values['uid'])) ) ? $account->mail : '',
      'plan_number' => '',
      'plan_type' => '',
      'status' => '',
      'created' => '',
      'changed' => '',
    );

    return parent::create($values);
  }

  /**
   * Overridden to generate auto data.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    if (!isset($transaction)) {
      $transaction = db_transaction();
      $started_transaction = TRUE;
    }

    try {
      global $user;

      $entity->is_new = !empty($entity->is_new) || empty($entity->{$this->idKey});
      // Set the timestamp fields.
      if ($entity->is_new) {
        if (empty($entity->created)) {
          $entity->created = REQUEST_TIME;
        }
      }
      else {
        // Otherwise if the entity is not new but comes from an entity_create()
        // or similar function call that initializes the created timestamp, uid,
        // and hostname values to empty strings, unset them to prevent
        // destroying existing data in those properties on update.
        if ($entity->created === '') {
          unset($entity->created);
        }
        if ($entity->uid === '') {
          unset($entity->uid);
        }
      }

      $entity->changed = REQUEST_TIME;

      $entity->revision_timestamp = REQUEST_TIME;
      $order->revision_hostname = ip_address();
      $entity->revision_uid = $user->uid;

      // Recalculate the entity total using the current line item data.

      if ($entity->is_new || !empty($entity->revision)) {
        // When inserting either a new entity or revision, $entity->log must be set
        // because {commerce_entity_revision}.log is a text column and therefore
        // cannot have a default value. However, it might not be set at this
        // point, so we ensure that it is at least an empty string in that case.
        if (!isset($entity->log)) {
          $entity->log = '';
        }
      }
      elseif (empty($entity->log)) {
        // If we are updating an existing entity without adding a new revision,
        // we need to make sure $entity->log is unset whenever it is empty. As
        // long as $entity->log is unset, drupal_write_record() will not attempt
        // to update the existing database column when re-saving the revision.
        unset($entity->log);
      }

      return parent::save($entity, $transaction);
    }
    catch (Exception $e) {
      if (!empty($started_transaction)) {
        $transaction->rollback();
        watchdog_exception($this->entityType, $e);
      }
      throw $e;
    }

    parent::save($entity, $transaction);
  }

  /**
   * Build content for response by view_mode
   *
   * @param object $entity
   * @param string $view_mode
   * @param string $langcode
   * @param array $content
   *   default content array.
   *
   * @return array
   *   drupal theme array
   */
  public function buildContent($entity, $view_mode = 'full', $langcode = NULL, $content = array()) {
    $wrapper = entity_metadata_wrapper('procurement_plan', $entity);
    // integrate display config
    $content['title'] = array(
      '#markup' => '<div >' . $entity->title .  '</div>',
    );

    $content['plan_number'] = array(
      '#markup' => '<div >' . $entity->plan_number . '</div>',
    );

    return parent::buildContent($entity, $view_mode, $langcode, $content);
  }
}

class ProcurementPlan extends Entity {
  protected function defaultLabel() {
    return $this->title;
  }

  protected function defaultUri() {
    return array('path' => 'procurement-plan/' . $this->identifier());
  }
}
